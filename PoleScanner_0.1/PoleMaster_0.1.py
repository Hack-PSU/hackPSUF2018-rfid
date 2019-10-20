# Prequistes:
# SPI: https://github.com/lthiery/SPI-Py
# sudo pip3 install rpi_ws281x adafruit-circuitpython-neopixel
#
###GLOBAL AND RESEVERES WARNING####
#global active, available
#global LockState
AdminKeys = ["key 1 serial","key 2 serial", "Key N serial"]
LockState = False
FirstTimeStartup = True
ServerParameters = [] #server prameters will be store as globs
#
####LIGHT RING STRUCTURE IMPORT###====================================================================================
import time
import board
import neopixel
import threading
###LOGIC IMPORT###
import urllib2
import sys
import os


####SYSTEM LOGIC####==================================================================================================

# --------------CORE LOGIC-------------------------

def core():
    light(0)
    global FirstTimeStartup
    CurrentTag = SearchforTag(FirstTimeStartup)  # not defined yet
    if boxLockCheck(currentTag) == True:
        main()

    critical, status, admit = SendToServer(CurrentTag) #bolean,string,bolean

    if critical:                                                        #may add Soft Reset Proceedure on multi-fail???
        print("WARNING: Crit Error from server reply:" + status)
        light(4)
        light(0)

    if admit == False:
        light(3)
        wait.time(5) #add to in between all
        light(0)
        main()
    else:
        light(2)
        light(0)
        main()
## --------------STARTUP LOGIC-------------------------
    def startUp(FirstTimeStartup):
        check = SearchforTag(FirstTimeStartup)
        if check != "-10000": #case return needs to be defined later
            print("STATUS: Setup Has concluded going into autonomous mode")
            restore() #X Not definded yet will load old config (10/20/19 completed -maz)
            light(2)
            return

        internet_test()
        Events = GetMealEvents() #Not definded yet Daninels  or andrews job

#       Print("INPUT REQUIRRED: \n")
#       Print("\n")

        Print(Events) # some kind of graphical unpack will go here when we know what were dealing with

        selection = input("INPUT REQUIRED: Select event ID as a ## number")


        while True:
            if selection in Events: ##needs fixed with more info

                print("STATUS: Config Choosen as" + selection)
                Con = input("INPUT REQUIRED: Confirm setup with any key, 'N' will abort all")
                if Con == "N":
                    sys.exit()
                print("STATUS: Setup has concluded going into autonomous mode")
                #save slection parmeters to global
                global ServerParameters
                ServerParameters = [Events[slection]] ###BUG CHECK after we know paremeters
                #save slection with save() function
                Save()
                break

            else:
                print("WARNING: Config " + selection + "Is not a valid selection")
                selection = input("INPUT REQUIRED: Select event ID as a ## number")

    ## --------------SAVE TO FILE--------------------------
    #A function that will Creates or Edit a text file in the Present working dir. that takes no arguments but using the current global varible for server parameters
    def Save():
        def Save():
            global ServerParameters
            Dir = os.getcwd()
            f = open(os.path.join(Dir, 'ServerParameters.txt'), 'w')
            f.write(str(ServerParameters))
            f.close()
    ## --------------LOAD FROM FILE-------------------------
    #A function that will open and read a text file in the Present working dir. that returns nothing but will store the server parameters for current global varible
    def Load():
        global ServerParameters
        Dir = os.getcwd()
        f = open(os.path.join(Dir, 'ServerParameters.txt'), 'r')
        out = f.read()
        ServerParameters = out.strip('][').split(', ')  ###MAJOR BUG### Needs a for loop for casting once we know what "parsmters" look like
        f.close()
        #print(ServerParameters)
        
    ## --------------RESTORE--------------------------------
    def restore():
        Load()
        global FirstTimeStartup
        FirstTimeStartup = False
    ## --------------INTERNET CHECK-------------------------

    def internet_test():

        print("STATUS: TESTING INTERNET")
        url = "http://google.com" #change to hack server
        try:
            urllib.request.urlopen(url)
        except urllib.error.URLError as e:
            print("WARNING: " + e.reason)
            print("ERROR: Network Down, soft reset recommended")
            time.wait(1500)
            sys.exit()

        print("STATUS: Connection Pass, we can see " + url)


# --------------LOCK AND ADMIN-------------------------
#Function takes atributes rfidserial as a string and AdminKeys as a list of stings, Returns Bol of lock status
def boxLockCheck(RfidSerial):
    global AdminKeys, LockState
    if RfidSerial in AdminKeys:
        light(2)
        LockState = not LockState
    if LockState == True:
        light(3)
        return True
    else:
        return False


####END SYSTEM LOGIC####==============================================================================================

####LIGHT RING STRUCTURE###===========================================================================================
# LED strip configuration:
LED_COUNT = 24  # Number of LED pixels.
LED_BRIGHTNESS = 10  # Set to 0 for darkest and 255 for brightest
LED_CHANNEL = 0
# --------------Custom-------------------------

# Change D18 to D## if switching pins, I recommend using a pin with PWM
pixels = neopixel.NeoPixel(board.D18, LED_COUNT)

# used to check if there is an ongoing pattern and if the next pattern can be started
# essentially:
# 	> pattern starts
#	> active = true, available = false
#	> next pattern inputted
#	> active = false
#	> ongoing pattern will immediately return from their function
#	> available = true
#	repeat
active = False
available = True

# --------------Patterns-------------------------


# turns of the LED at i
def off(i):
    pixels[i] = (0, 0, 0)


# Set all pixels to the RGB color value
def setColor(red, green, blue):
    for i in range(LED_COUNT):
        pixels[i] = (red, green, blue)
    global available
    available = True



# Used if wait_ms is high, allows checking if active = true while sleeping
def sleepChecking(wait_ms):
    current = 0
    while active and current < wait_ms:
        time.sleep(10 / 1000)
        current += 10



# Displays a circular moving pattern of color
def circleColor(red, green, blue, wait_ms=50):
    # determines how many LED will be on and circulating
    start = 0
    end = 8

    # turns on the initial LED from start to end
    for j in range(LED_COUNT):
        off(j)
    for i in range(start + 1, end):
        pixels[i] = (red, green, blue)

    # afterwards, continue circulating the LED
    while active:
        if (start > LED_COUNT - 2):
            start = -1
        if (end > LED_COUNT - 2):
            end = -1
        start += 1
        end += 1
        pixels[end] = (red, green, blue)
        off(start)
        time.sleep(wait_ms / 1000.0)
    global available
    available = True


# Displays a breathing effect of color
def breathingColor(red, green, blue, wait_ms=50):
    brightness = 0
    increasing = True  # determines if brightness is increasing or decreasing
    while active:
        if brightness == LED_BRIGHTNESS: increasing = False
        if brightness == 0: increasing = True

        if increasing:
            brightness += 1
        else:
            brightness -= 1

        # Currently, only applies to blue since it is only for standby
        blue = brightness
        for i in range(LED_COUNT):
            pixels[i] = (red, green, blue)
        time.sleep(wait_ms / 1000.0)
    global available
    available = True


# Displays a blinking pattern of color
def blinkingColor(red, green, blue, wait_ms=1000):
    while active:
        for j in range(LED_COUNT):
            off(j)
        sleepChecking(wait_ms)
        for i in range(LED_COUNT):
            pixels[i] = (red, green, blue)
        sleepChecking(wait_ms)
    global available
    available = True

# --------------States----------------------------------​

# Displays Breathing Blue when on standby
def LED_standby():
    breathingColor(0, 0, LED_BRIGHTNESS)


# Displays circling Yellow when processing
def LED_processing():
    processing = True
    circleColor(LED_BRIGHTNESS, int(LED_BRIGHTNESS / 2), 0)
    processing = False


# Displays green when user is accepted
def LED_accepted():
    setColor(0, LED_BRIGHTNESS, 0)



# Displays red when user is denied
def LED_denied():
    setColor(LED_BRIGHTNESS, 0, 0)



# Displays blinking Yellow when an error occurs
def LED_error():
    blinkingColor(LED_BRIGHTNESS, int(LED_BRIGHTNESS / 2), 0)

# Turns all LED off and allows safe exit
def LED_exit():
    setColor(0, 0, 0)

# --------------Light Function-------------------------


# Signals
#    0 - standby
#    1 - processing
#    2 - accepted
#    3 - denied
#    4 - error
#	 5 - exit
def light(signal):
    switcher = {
        0: LED_standby,
        1: LED_processing,
        2: LED_accepted,
        3: LED_denied,
        4: LED_error,
        5: LED_exit
    }
    func = switcher.get(signal, LED_error)

    # explained at active and available insatiation, scroll up
    global active
    global available
    if active:
        active = False
        while not available:
            time.sleep(10 / 1000)
    active = True
    available = False
    t = threading.Thread(target=func, daemon=True)
    t.start()
    return

# ----------------------------------------------------

# testing
# if __name__ == '__main__':
#     print('Press Ctrl-C to quit.')
#     while True:
#         state = input()
#         light(int(state))
####END LIGHT RING STRUCTURE###===========================================================================================

###DAS MAIN####
def main():
    global FirstTimeStartup
    if FirstTimeStartup:
        startUp(FirstTimeStartup)
        FirstTimeStartup = False
    
    Core()





if __name__ == '__main__':
    main()




#                            .-----.
#                          .'       `.
#                         :           :
#                         :           :  SPOOPY CODE FOR POLE
#                         '           '
#          |~        www   `.       .'
#         /.\       /#^^\_   `-/\--'
#        /#  \     /#%    \   /# \
#       /#%   \   /#%______\ /#%__\
#      /#%     \   |= I I ||  |- |
#      ~~|~~~|~~   |_=_-__|'  |[]|
#        |[] |_______\__|/_ _ |= |`.
# ^V^    |-  /= __   __    /-\|= | :;
#        |= /- /\/  /\/   /=- \.-' :;
#        | /_.=========._/_.-._\  .:'
#        |= |-.'.- .'.- |  /|\ |.:'
#        \  |=|:|= |:| =| |~|~||'|
#         |~|-|:| -|:|  |-|~|~||=|      ^V^
#         |=|=|:|- |:|- | |~|~|| |
#         | |-_~__=_~__=|_^^^^^|/___
#         |-(=-=-=-=-=-(|=====/=_-=/\
#         | |=_-= _=- _=| -_=/=_-_/__\
#         | |- _ =_-  _-|=_- |]#| I II
#         |=|_/ \_-_= - |- = |]#| I II
#         | /  _/ \. -_=| =__|]!!!I_II!!
#        _|/-'/  ` \_/ \|/' _ ^^^^`.==_^.
#      _/  _/`-./`-; `-.\_ / \_'\`. `. ===`.
#     / .-'  __/_   `.   _/.' .-' `-. ; ====;\
#    /.   `./    \ `. \ / -  /  .-'.' ====='  >
#   /  \  /  .-' `--.  / .' /  `-.' ======.' /
