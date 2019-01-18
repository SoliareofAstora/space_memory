import space_enviro.spaceLib as env


# https://stackoverflow.com/questions/13207678/whats-the-simplest-way-of-detecting-keyboard-input-in-python-from-the-terminal/31736883
# raw_input / input are asynchronus - good for game setup
#

b = env.hello("Hello World")
b.greet()
while b.stillPlaying():
    b.display()

print("Dome")