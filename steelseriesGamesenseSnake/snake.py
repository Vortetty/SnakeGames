import os
os.system("python -m pip install -r requirements.txt")


import asyncio
import keyboard
import gamesense
import math
import time
import random
import requests
from switchcase import switch

version = "1.0.0"
gs = gamesense.GameSense("SNAKE_V2", f"Snake v{version}", "Winter (github.com/vortetty)")

resolution = [128, 40]
screen = [ 0 for _ in range(resolution[0]*resolution[1]) ]

def conv_bitmap_array(arr:list):
    """
    Convert a numpy array to the pixel array requested by steelseries engine
    """
    if len(arr)<resolution[0]*resolution[1]:
        rows=[]
        for r in range(0,len(arr),len(arr)//resolution[1]):
            rows.append(arr[r:r+len(arr)//resolution[1]-1])
        for i,r in enumerate(rows):
            if len(r)<resolution[0]:
                rows[i].extend([0 for i in range(resolution[0]-len(r))])
        if len(rows)<resolution[1]:
            rows.extend([[0 for i in range(resolution[0])] for j in range(resolution[1]-len(rows))])
        arr=[a for b in rows for a in b]
                

    new_arr=[]
    temp_byte=''
    for c in arr:
        if int(c)>1:c=1
        temp_byte+=str(c)
        if len(temp_byte)==8:
            new_arr.append(int(temp_byte,2))
            temp_byte=''

    
    return new_arr

def setPixel(x: int, y: int, value: bool):
    screen[x + y * resolution[0]] = value

def gameLoop():
    gs.register_game()
    gs.bind_event("SNAKE_UPDATE", handlers=[
        {
            "device-type": f"screened-{resolution[0]}x{resolution[1]}",
            "mode": "screen",
            "zone": "one",
            "datas": [
                {
                    "has-text": False,
                    "image-data": conv_bitmap_array(screen)
                }
            ]
        }
    ])

    gs.bind_event("SNAKE_GAME_OVER", handlers=[
        {
            "device-type": f"screened-{resolution[0]}x{resolution[1]}",
            "mode": "screen",
            "zone": "one",
            "low": 0,
            "high": resolution[0]*resolution[1],
            "datas": [
                {
                    "has-text": True,
                    "prefix": "Final score: "
                }
            ]
        }
    ])

    gs.bind_event("SNAKE_TITLE", handlers=[
        {
            "device-type": f"screened-{resolution[0]}x{resolution[1]}",
            "mode": "screen",
            "zone": "one",
            "low": 0,
            "high": resolution[0]*resolution[1],
            "datas": [
                {
                    "has-text": True,
                    "prefix": f"Snake v{version[:-1]}"
                }
            ]
        }
    ])

    gs.send_event("SNAKE_TITLE", {
       "value": int(version[-1]),
    })

    while not keyboard.is_pressed("enter"):
        gs.send_heartbeat()

    gameover = False
    snekLoc = [ [2, resolution[1]-3] ]
    foodLoc = [ 2, 2 ]
    snekDir = 0
    lastUpdate=time.time()
    score = 0

    for x in range(resolution[0]):
        for y in range(resolution[1]):
            if x == 0 or x == resolution[0]-1 or y == 0 or y == resolution[1]-1:
                setPixel(x, y, 1)

    while not gameover:
        if keyboard.is_pressed("up"): snekDir = 0
        if keyboard.is_pressed("down"): snekDir = 1
        if keyboard.is_pressed("left"): snekDir = 2
        if keyboard.is_pressed("right"): snekDir = 3

        if time.time()-lastUpdate >= 0.5:
            for case in switch(snekDir):
                newLoc = snekLoc[-1].copy()

                if case(0):
                    newLoc[1] -= 1
                elif case(1):
                    newLoc[1] += 1
                elif case(2):
                    newLoc[0] -= 1
                elif case(3):
                    newLoc[0] += 1
                elif case(4):
                    gameover = True
                    losemessage = f"Game canceled, final score: {score}"
                
                if newLoc == foodLoc:
                    snekLoc.append(newLoc)
                    foodLoc = [random.randrange(resolution[0]-2), random.randrange(resolution[1]-2)]
                    while foodLoc in snekLoc:
                        foodLoc = [random.randrange(2, resolution[0]-2), random.randrange(resolution[1]-2)]
                    score += 1
                elif newLoc in snekLoc:
                    gameover = True
                    gs.send_event("SNAKE_GAME_OVER", {
                        "value": score,
                    })
                elif not (0 < newLoc[0] and newLoc[0] < resolution[0]-1) or not (0 < newLoc[1] and newLoc[1] < resolution[1]-1):
                    gameover = True
                    gs.send_event("SNAKE_GAME_OVER", {
                        "value": score,
                    })
                else:
                    snekLoc.append(newLoc)
                    tmp = snekLoc.pop(0)
                    setPixel(tmp[0], tmp[1], 0)

                setPixel(snekLoc[-1][0], snekLoc[-1][1], 1)
                setPixel(foodLoc[0], foodLoc[1], 1)

                if not gameover:
                    gs.send_event("SNAKE_UPDATE", {
                        "value": next(gs.value_cycler),
                        "frame": {
                            f"image-data-{resolution[0]}x{resolution[1]}": conv_bitmap_array(screen)
                        }
                    })

                lastUpdate = time.time()

gameLoop()
