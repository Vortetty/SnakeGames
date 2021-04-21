import discord
from discord.ext import commands
import requests
import time
import asyncio
from operator import eq

intents = discord.Intents.all()
bot = commands.Bot(command_prefix="s!", intents=intents)
token = "tokenHere"

async def addReactions(reactions, message):
    await asyncio.gather(
        *[message.add_reaction(emoji) for emoji in reactions]
    )

def switch(value, comp=eq):
    return [lambda match: comp(match, value)]

@bot.event
async def on_ready():
    print('Logged in as')
    print("Name:", bot.user.name + "#" + bot.user.discriminator)
    print("ID:", bot.user.id)

    print('Servers connected to:')
    for guild in bot.guilds:
        channel = None
        if len(await guild.invites()) > 0:
            channels = await guild.invites()
            channel = channels[0].url
        else:
            channels = guild.text_channels
            channel = await channels[0].create_invite(reason=None, max_age=3600)
            channel = channel.url
        print(f"{guild.name}: ({guild.id}, {channel})")

@bot.command()
async def snake(ctx, group: bool = False):
    reaction: discord.RawReactionActionEvent = None
    gameover = False
    except: pass
    score = 0

    foodLoc = [1, 1]
    snekLoc = [ [1, 8] ]

    dirs = {
        "⬆": 0, 
        "⬇": 1, 
        "⬅": 2, 
        "➡": 3,
        "🛑": 4
    }

    invalidMoves = [
        1, 0, 3, 2, 4
    ]

    dirStrs = ["up", "down", "left", "right", "cancel"]

    startTime = time.time()

    emote = None
    direction = 0
    losemessage = ""

    field = [
        [ char for char in "⬜⬜⬜⬜⬜⬜⬜⬜⬜⬜"],
        [ char for char in "⬜⬜⬜⬜⬜⬜⬜⬜⬜⬜"],
        [ char for char in "⬜⬜⬜⬜⬜⬜⬜⬜⬜⬜"],
        [ char for char in "⬜⬜⬜⬜⬜⬜⬜⬜⬜⬜"],
        [ char for char in "⬜⬜⬜⬜⬜⬜⬜⬜⬜⬜"],
        [ char for char in "⬜⬜⬜⬜⬜⬜⬜⬜⬜⬜"],
        [ char for char in "⬜⬜⬜⬜⬜⬜⬜⬜⬜⬜"],
        [ char for char in "⬜⬜⬜⬜⬜⬜⬜⬜⬜⬜"],
        [ char for char in "⬜⬜⬜⬜⬜⬜⬜⬜⬜⬜"],
        [ char for char in "⬜⬜⬜⬜⬜⬜⬜⬜⬜⬜"]
    ]

    field[foodLoc[1]][foodLoc[0]] = "🍎"
    for i in snekLoc:
        field[i[1]][i[0]] = "🟨"
    field[snekLoc[-1][1]][snekLoc[-1][0]] = "😶"
    
    message: discord.message = await ctx.send("Snake:\n" + "\n".join("".join(i) for i in field) + f"\nScore: {score}")
    await addReactions(["⬆", "⬇", "⬅", "➡", "🛑"], message)

    while not gameover:
        try:
            reaction = await bot.wait_for('raw_reaction_add', timeout=1, check=lambda x:x.member == ctx.author or group)
        except asyncio.TimeoutError:
            pass
        else:
            pass
        
        if reaction:
            emote = reaction.emoji
            direction = dirs[str(emote)]

            await message.remove_reaction(emote, reaction.member)

        if time.time() - startTime >= 1:
            startTime = time.time()

            for case in switch(direction):
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
                
                if not gameover:
                    if newLoc == foodLoc:
                        snekLoc.append(newLoc)
                        foodLoc = [random.randrange(10), random.randrange(10)]
                        while foodLoc in snekLoc:
                            foodLoc = [random.randrange(10), random.randrange(10)]
                        score += 1
                    elif newLoc in snekLoc:
                        gameover = True
                        losemessage = f"Snake collided with itself, final score: {score}"
                    elif not (-1 < newLoc[0] and newLoc[0] < 10) or not (-1 < newLoc[1] and newLoc[1] < 10):
                        gameover = True
                        losemessage = f"Snake collided with wall, final score: {score}"
                    else:
                        snekLoc.append(newLoc)
                        snekLoc.pop(0)
                        
                field = [
                    [ char for char in "⬜⬜⬜⬜⬜⬜⬜⬜⬜⬜"],
                    [ char for char in "⬜⬜⬜⬜⬜⬜⬜⬜⬜⬜"],
                    [ char for char in "⬜⬜⬜⬜⬜⬜⬜⬜⬜⬜"],
                    [ char for char in "⬜⬜⬜⬜⬜⬜⬜⬜⬜⬜"],
                    [ char for char in "⬜⬜⬜⬜⬜⬜⬜⬜⬜⬜"],
                    [ char for char in "⬜⬜⬜⬜⬜⬜⬜⬜⬜⬜"],
                    [ char for char in "⬜⬜⬜⬜⬜⬜⬜⬜⬜⬜"],
                    [ char for char in "⬜⬜⬜⬜⬜⬜⬜⬜⬜⬜"],
                    [ char for char in "⬜⬜⬜⬜⬜⬜⬜⬜⬜⬜"],
                    [ char for char in "⬜⬜⬜⬜⬜⬜⬜⬜⬜⬜"]
                ]

                field[foodLoc[1]][foodLoc[0]] = "🍎"
                for i in snekLoc:
                    field[i[1]][i[0]] = "🟨"
                field[snekLoc[-1][1]][snekLoc[-1][0]] = "😶"

                await message.edit(content="Snake:\n" + "\n".join("".join(i) for i in field) + f"\nScore: {score}")
    await message.edit(content=losemessage)
    await message.clear_reactions()

@bot.event
async def on_message(message):
    if message.author == bot.user:
        return

    await bot.process_commands(message)

    try: await message.delete()
    except: pass

@bot.event
async def on_raw_message_edit(payload):
    message = await bot.get_channel(payload.channel_id).fetch_message(payload.message_id)
    await on_message(message)

bot.run(token)