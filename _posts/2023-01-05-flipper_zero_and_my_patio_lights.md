---
layout: post
title: 'Flipper Zero and My Patio Lights'
author: pdavid
excerpt_separator: <!--more-->
datatable: true
img-path: /assets/img/2023-01-05
content-path: 2023-01-05
---
{% include badges.md %}
Having a massive (100ft) LED strip for my patio lights was a big goal when we moved into our new apartment.  But we really wanted to have a little more flexibility than provided by the built-in (RF-only) controller.  Using the Flipper Zero, I was able to eventually capture all the remote's buttons to control the lights with a Raspberry Pi Pico W, a CC1101 module, and a lot of trial and error.

<!--more-->
{% capture img0 %}{{ page.img-path }}/banner.jpg{% endcapture %}
{% include image.html file=img0 caption="" %}

## Overall Stages of Development
{: .no_toc}

- TOC
{:toc}

{::comment}
| Step | Description | Result |
|:----:|:----------- |:-----:|
| 1 | [Overview of Setup & Parts Used](#1---overview-of-setup--parts-used-) | {{ site.data.badges.info }} |
| 2 | [Recording the Remote Buttons](#2---recording-the-remote-buttons-) | {{ site.data.badges.ok }} |
| 3 | [Research of Signal Protocols](#3---research-of-signal-protocols-) | {{ site.data.badges.wrong_turn }} |
| 4 | [CC1101 & Pico W Setup](#4---cc1101--pico-w-setup-) | {{ site.data.badges.ok }} |
| 5 | [Transmitting w/ Micropython](#5---transmitting-w-micropython-) | {{ site.data.badges.failed }} |
| 6 | [Try again w/ Arduino C++](#6---trying-again-w-arduino-c-) | {{ site.data.badges.ok }} |
| 7 | [Pretending at Signal Analysis](#7---pretending-at-signal-analysis-) | {{ site.data.badges.good_guess }} |
| 8 | [Refining Signal Data Storage](#8---refining-signal-data-storage-) | {{ site.data.badges.ok }} |
| 9 | Attempting to Use a Preamble | {{ site.data.badges.failed }} |
| 10 | Good Enough! | {{ site.data.badges.ok }} |
| 11 | Random Insight into Behavior & Encoding | {{ site.data.badges.good_guess }} |
| 12 | Finally Encoding Original Recordings | {{ site.data.badges.ok }}
| 13 | Remote Buttons All Recorded | {{ site.data.badges.ok }}
{: .table .table-striped }
{:/comment}

<hr />
## 1 - Overview of Setup & Parts Used {{ badge_info }} ##

For this project, I had an already existing customized 100-foot long RGB light strip. The customization is probably good for a post on its own, but at a high level, the lighting came with a waterproof LED strip, an RF controller, and an RF remote control. Unfortunately the LED strip wasn't waterproof, so I customized it by building the controller into a waterproof box and extending the power cord so I could leave it out on my porch through all weather conditions without any major concerns.

I'd also just recently gotten into poking at Raspberry Pi Picos. I'd used Arduinos and Particle Argons in the past on projects but liked the idea of running Python on a microcontroller because I'm more familiar with Python than C++. (You'll see why that didn't work out for this project soon. Ah, well.) So I figured I'd go RPi Pico W with the CC1101 transceiver (the same one that's in the Flipper Zero). I had already experimented a bit and figured out that the Flipper can emulate the remote, so I knew having the same transceiver would allow me to do it myself if I could figure it out.

**<u>1.1 - Parts List</u>**

* LED Light Strip
    * Brillihood 100ft/30m RGB Neon Rope Light <sub>[Link](https://www.amazon.com/Brillihood-Linkable-Waterproof-Multicolor-Controller/dp/B08HLP1N45)</sub>
* Required Items - RF Transmitter
    * Raspberry Pi Pico W <sub>[Link](https://www.raspberrypi.com/products/raspberry-pi-pico/?variant=raspberry-pi-pico-w)</sub>
    *  CC1101 Wireless RF Transceiver <sub>[Link](https://www.amazon.com/gp/product/B01DS1WUEQ)</sub>
* Optional Items - Debugging / prototyping
    * FlipperZero <sub>[Link](https://flipperzero.one/)</sub>
    * GeeekPi Raspberry Pi Pico/Pico W Terminal Expansion Board <sub>[Link](https://www.amazon.com/gp/product/B0998594PL)</sub>
    * Miscellaneous Jumper Wires <sub>[Example](https://www.amazon.com/gp/product/B07GD2BWPY)</sub>

I listed the GeeekPi board as temporary because I could go through and build a case for my RF transmitter setup. I have not done so, since it was very very very late at night when I got everything working and I now have it all still connected using the GeeekPi board. It'll probably stay that way until after the holidays.

<hr />
## 2 - Recording the Remote Buttons {{ badge_ok }} ##
This is the remote that I attempted to emulate. It's a standard RF no-brand remote that comes with tons of different LED strips. Honestly, the colors mostly didn't match the actual light strip when lit, but since I wasn't using it for color fidelity checks, it was good enough for me.

{% capture img1 %}{{ page.img-path }}/remote.jpg{% endcapture %}
{% include image.html file=img1 caption="Remote for LED Strip" %}

So, there's a lot of stuff already written on using the Flipper Zero for [recording RF signals](https://docs.flipperzero.one/sub-ghz). I basically read this, assumed I knew everything I needed to know, and just pressed buttons until I got something to work.

**<u>2.1 - Finding the frequency of the remote</u>**

I started with the sub-GHz [frequency analyzer](https://docs.flipperzero.one/sub-ghz/read#bwvDc), which happened to default to an AM frequency that showed up with my remote. I started the  analyzer, and pressed a few of the remote buttons. When doing so, it popped up at ~433MHz, so I was pretty positive that if the analyzer could see it, I could (at worst) read the raw signal and play it back. At best, it could decode whatever the signal is and provide me the protocol to use, simplifying the full remote's emulation.

{% capture img2 %}{{ page.img-path }}/siganalyzer.jpg{% endcapture %}
{% include image.html file=img2 caption="Signal Analyzer" %}

**<u>2.2 - Recording the raw signals</u>**

Once I knew the frequency, I switched to Flipper's [`Read RAW`](https://docs.flipperzero.one/sub-ghz/read-raw) mode to verify if it's capable of playing back a signal. I started the recorder and pressed the orange light button remote, then stopped the recorder and saved it to the Flipper. It showed me a spike on the screen, so I assumed everything was good. 

{% capture img3 %}{{ page.img-path }}/recordraw.jpg{% endcapture %}
{% include image.html file=img3 caption="Raw Signal Recording" %}

Then I changed the light strip to another color, and used Flipper's `Send` mode to play it back. The light changed, meaning the CC1101 chip worked! So, I now had my fallback plan confirmed to be an option. Since I'll be using the signal from another device, I grabbed the raw recording file from the Flipper.

```cpp
{% include_relative {{ page.content-path }}/raw_file.sub %}
```

**<u>2.3 - Recording decoded signals</u>**

For my easiest method of emulating the remote, I'd prefer the decoded signals. This would prevent trimming and cleaning up the raw signal recordings to get the noise out, and would mean I could use a simple hex number or string (depending on the sent data).  So now I moved to trying to use Flipper's [`Read`](https://docs.flipperzero.one/sub-ghz/read) functionality.

I repeated the same way as the raw recording:  Start listening, press the orange button, and when it detected it and decoded, I stopped the recording and saved the file. I used `Send` to play it back, and it correctly changed the lighting back to orange.

{% capture img4 %}{{ page.img-path }}/recorddecoded.jpg{% endcapture %}
{% include image.html file=img4 caption="Decoded Signal Recording" %}

Since it decoded _something_ successfully, I went through and recorded all the buttons.  They were all the same format, with a few bytes of hex different, so that was promising.

```cpp
{% include_relative {{ page.content-path }}/decoded_file.sub %}
```

If I could emulate one of them from the protocol, I could possibly emulate all of them based on their bytes as opposed to dealing with raw signals. That's some positive motion!

<hr />
## 3 - Research of Signal Protocols {{ badge_wrong_turn }} ##

Okay, so here's the first real misdirection that I took. The information I gathered from here was beneficial in helping understand what's possible, but honestly was otherwise a large time sink for not much output. Here's the summary takeaways that were beneficial:

> 1. I couldn't find a solid explanation of the `Princeton 24-bit` protocol that Flipper refers to. Maybe some other people know what this is, but I was unable to find any real details on how this results in actual raw signal transmissions. This meant I was going to have to write my own protocol encoder due to lack of knowledge on where to find support.
> 2. The protocol refers to some `ASKOOK` portion of the protocol. The `ASK` was easily researched as "amplitude shift keying" and the `OOK` was a more specific version of `ASK` known as "on-off keying". This meant that I wasn't dealing with a frequency modulation at all, but just an on/off signal. If I was able to turn the radio on and off properly, the signal could be emulated.
> 3. The CC1101 chip supports synchronous mode and asynchronous mode for transmission / reception. I'm ignoring the reception since the receiver will actually be the LED strip controller out on my porch. But to finely control the transmission to match my signals I would have to use the async mode. Without protocol details, I couldn't rely on using the synchronous setup and letting the CC1101 handle a specific protocol.
> 4. Flipper's RAW signal encoding is actually very simple:  A string of integers with positive numbers meaning HIGH and negative numbers meaning LOW/OFF. The integers are a measure of microseconds that the signal was at that level. So a RAW signal of `100 -200 100` means ON for 100 microseconds, off for 200 microseconds, and on again for 100 microseconds. Using async mode, this is really do-able via a microcontroller if we just paste in the raw signals as an array.

So net result was that I'd have to go back and plan on using the raw signal mode for now, and see if I can get that working.

<hr />
## 4 - CC1101 & Pico W Setup {{ badge_ok }} ##

There's a [lot](https://electronoobs.com/eng_arduino_tut98.php) of [different](https://asksinpp.de/Grundlagen/01_hardware.html#microcontroller) [sites](https://www.aeq-web.com/arduino-10mw-cc1101-ism-rf-transceiver/?lang=en) that discuss using the CC1101 chip with arduinos, Raspberry Pis, and other controllers. However, none of them discussed using them with an Raspberry Pi Pico. I also noticed that my CC1101 chip had 8 pins instead of the 10 most of the examples I saw were done with.  I found [this comment](https://www.amazon.com/gp/customer-reviews/R21PKB3UFT5ITK/ref=cm_cr_dp_d_rvw_ttl?ie=UTF8&ASIN=B00XDL9E64) on a different CC1101 transceiver which seemed to provide an 8-pin pinout:

| # | Pin | # | Pin |
| --- | --- | --- | --- |
| 1 | GND | 2 | VCC |
| 3 | GDO0 | 4 | CSN |
| 5 | SCLK | 6 | SDI |
 7 | SDO | 8 | GDO2 |
{: .table .table-striped }

So, using this pinout and my Pico W, I did the following wiring using the GeeekPi Board to allow me to see activity on the different pins while working on the software side.

| # | CC1101 | PicoW | # | CC1101 | PicoW |
| --- | --- | --- | --- | --- | --- |
| 1 | GND | GND | 2 | VCC | 3v3 |
| 3 | GDO0 | GPIO20 | 4 | CSN | GPIO17 |
| 5 | SCLK | GPIO18 | 6 | SDI | GPIO19 |
| 7 | SDO | GPIO16 | 8 | GDO2 | none |
{: .table .table-striped }

I chose to use the default SPI0 setup on the Pico W (at the time - it's since been moved to lower GPIO pins).  I also used GDO0 on GPIO20 just to keep all the wiring together and allow for more tight cabling in a more permanent setup once done working on the emulation.

{% capture img1 %}{{ page.img-path }}/diagram.jpg{% endcapture %}
{% include image.html file=img1 caption="Wiring Diagram" classes="mw-100" %}

<hr />
## 5 - Transmitting w/ Micropython {{ badge_failed }} ##

Since I was working on a transmit-only project, I was fine using the sample CC1101 micropython library for testing, but would have to modify it to properly operate in a raw, GDO0-driven mode. I came across [this library](https://edugit.org/mwinter/micropython-cc1101) by Martin Winter, and figured I could at least use the library to verify if my wiring was working properly. I loaded up my Flipper in `Read RAW` mode and ran the example program, and saw a spike. Hooray! The wiring was good!

I then went about trying to understand the raw library, and it was, to say the least, mostly incomprehensible. Not because of the library itself, but because of the large number of variables that are involved in configuring the CC1101 chip itself. The first 100+ lines of the library are basically just defining all the addresses of the different chip settings, and later in the library is a `regConfigSettings` method that configures the chip for the FIFO-based transmission the library is setup to use. I could modify the configuration of the library locally in my project by just updating this method with whatever appropriate settings are needed.

Lots of searching around the internet, including reading the majority of the configuration tables and options for the chip found [in the data sheet](https://www.ti.com/lit/ds/symlink/cc1101.pdf) gave me an idea of several of the settings needed to be modified, but no matter what I did, I could not get it to transmit ANYTHING in response to a high-signal on the GDO0 pin. I even went so far as to attempt to use the [TI SmartRF Flash Programmer](https://www.ti.com/tool/FLASH-PROGRAMMER) tool to attempt to export out all the correct settings, and that still didn't get a functional working transmission. 

I basically threw in the towel in the MicroPython effort here, as I was unable to come up with a working solution. I clearly didn't know enough about what I was doing.

<hr />
## 6 - Trying again w/ Arduino C++ {{ badge_ok }} ##

So, giving up on the MicroPython meant I had to revert back to C++, Arduino-style. I'm honestly terrible at C++, but it's a programming language, so I can make do. By moving to C++, I _was_ able to find a clear GDO0-driven example which I could cannibalize for my purposes. [This project](https://github.com/simondankelmann/Esp32-SubGhz/blob/main/Esp32/Esp32-SubGhz/Esp32-SubGhz.ino) by Simon Dankelmann had a great example of using the raw Flipper signals to transmit via the async GDO0 setup, which is steps away from what I wanted.

The file read raw flipper files from an SD card and transmitted all of them. Since I'm not using an SD card, I had to translate what the example was doing and to pass in an array for the raw format. So I basically ripped out the `sendSamples` method and a bunch of the setup stuff. Basically, Simon seemed to have figured out all the CC1101 settings that I needed and configured that using the [SmartRC library](https://github.com/LSatan/SmartRC-CC1101-Driver-Lib) by LSatan.

```cpp
{% include_relative {{ page.content-path }}/testing_rawsend.ino%}
```

When ran on the Pico, it worked! I saw the signal on Flipper's raw recorder. I also noticed that my patio lights flipped to orange, so it definitely transmitted the raw signal successfully!

I could leave the project here, and simply record the red and green button signals, translate them to C++ arrays like the sample above, and then play them back on a loop, but that sounded like a lot of work. It also didn't really get me something that I could use in the future, as I'd have to record ALL of the buttons on the remote and the signals probably recorded noise that I'm now replaying every time, etc. etc. etc. So let's now start cleaning up this hot mess that I got working.

<hr />
## 7 - Pretending at Signal Analysis {{ badge_good_guess }} ##

Now tht I know I can replay the raw signals, I wanted to isolate out the clear signal in the noise of the full RAW recording. This would allow me to send the signals as fast as possible without the dead air that was at the beginning and end as I manually started and stopped recording on the Flipper.

So going back to the RAW recording file, I made a couple assumptions and worked from there:

> 1. There will be a large NEGATIVE area in the RAW file format at the beginning and end of the actual signal. The remote should be NOT sending when the button isn't pressed, so in theory, the signal should be all NEGATIVE and then whatever +-+-+- pattern for the signal, and then all NEGATIVE when done.
> 2. If I trim off something important from the actual signal, the patio lights will not respond when played back. So I would guess at areas of the signal to delete out, replay and see if the patio responds. If it does, I didn't trim out anything important. If it doesn't respond, then something in what I deleted was important and I should try trimming a smaller bit out.

Using these two assumptions, I started by just eyeballing for large numbers and seeing if they were negative. Unfortunately for me, background noise shows up in the raw recording as positive and negative numbers, and since amplitude isn't captured, it's hard to tell what is valid. So I searched for large 5+ digit numbers that are negative using regex, and started trimming.  Eventually, I was able to trim the signal down to a short (~50 segment) string of integers through sheer trial and error.

```cpp
{% include_relative {{ page.content-path }}/raw_file_trimmed.sub %}
```

I cleaned that up and got it into my code. I left it running on my Pico to just emulate the button press every two seconds, and it... didn't always work. It worked a lot, but not always. I went back to the raw file to try to figure out why it wasn't always working and noticed something else.  There was a consistently long NEGATIVE segment right at the end of the signal, but I saw a (close to) same length NEGATIVE segment four more times in the original button press signal. So I suspected that there was a repeated transmission going on on each button press. I updated my code to loop 5 times to match, and I saw a marked impovement in reliability of the patio responding. It would still flake out a bit, but not nearly as much.

Now that I know I have a mostly working chunk of signal, and that it repeated, I had something I really wanted to fix. I've been pasting in a RAW array of integers that were recorded, and the trimming trial and error was very, very slow. I didn't want to do that too much, but also I knew that the recording is a lossy function. I suspected I had repeated signals, but I didn't actually see any real repeating sequences in the Flipper RAW file, so I knew that the signal wasn't actually clean.

<hr />
## 8 - Refining Signal Data Storage {{ badge_ok }} ##

I was looking at the data and realized something else. There was a part of the DECODED Flipper file I wasn't sure what it meant exactly, and I couldn't find anything specific about it. Specifically, it was the `TE: 268` line. There was an interesting thing about it though. It seemed to be pretty close to a lot of the integers in the RAW recording that I trimmed down. Specifically, I was seeing a lot of "close to 268" values. I also checked and 3x268=804, so I was also seeing a lot of those too in the signal. As you can see below, it's pretty consistently a `3` or a `1`. The extra on the end I considered just the normal start / stop of the signal.

```cpp
{% include_relative {{ page.content-path }}/raw_file_converted.sub %}
```

What I realized was that I could probably break down the signal into these values (3s and 1s) and then just try multiplying by that 268. That would drastically reduce the ugly long arrays of integers that my code would need, and would also ensure a much cleaner signal since all the highs/lows would be held for a consistent length of time across the board. I'd still have to record and trim all the raw signals, but I could at least make my code easier and simplify the arrays used.

So I went about it and did the work for my orange signal. The array became very clearly a simple array of 1s, 3s, and a closing pause.

```cpp
{% include_relative {{ page.content-path }}/testing_trimmed_and_converted.ino %}
```

<hr />
## 9 - Attempting to Use a Preamble {{ badge_failed }} ##

With the above, the signal was much clearer to see, so I decided to do several more buttons and see if I could see a pattern. This paid off, as it was clear there was a pattern related to the decoded signal values and the raw patterns. I was able to identify a preamble of the same samples for the first ~30 samples in each signal. The thought was that I could reduce the sample array some more by only including the significant samples and prepending the samples from the preamble. I rebuilt my code and re-wrote the `sendSamples` function to do that.

```cpp
{% include_relative {{ page.content-path }}/testing_preamble.ino %}
```

However, even though the code compiled, it never transmitted data, and I would see my Pico's COM port drop off my computer when pushed. I suspect that there is something related to using `sizeof` inside of functions, which I remember vaguely being a bad thing, but was unable to properly debug. Since I was kinda deep down a rabbit hole of guessing and am not a great C++ developer (at all), I just decided to drop the preamble attempt and move on to getting the couple colors (red and green) that I wanted recorded and built into the tooling.

<hr />
## 10 - Good Enough! {{ badge_ok }} ##

So I had my process:

1. Record RAW signal from the remote button for the RED and GREEN buttons.
2. Find the important ~50 samples in the RAW signal.
3. Convert the RAW samples to the `1s` and `3s` format.
4. Add to the code and update the runner to alternate between the colors.

I sat down and spent the night doing this, and was able to get a working function. Below is the fully functional Green/Red swapping of lights by replicating the RAW signal using my array format.

```cpp
{% include_relative {{ page.content-path }}/testing_color_swapping.ino %}
```

<hr />
## 11 - Random Insight into LED Strip Behavior {{ badge_good_guess }} ##

This next section involves me walking away being happy with what I got working and basically assuming I was done. There were two things that bothered me:

> 1. That the colors just *snap* to green and red and back. Since I'm basically emulating the remote, it'd be nice if I could get it to nicely fade between green and red and back, but there's only the Fade button, and that's a rainbow fade through the colorwheel.
> 2. That I'm still dealing with the RAW signals. I didn't really want to go through and manually clean, trim, and convert all the raw signals to `1s` and `3s`, so if I could figure out the HEX decoded signals and how to play them back via my code, then I can easily record the decoded signals for all the buttons and make them usable for future patio lighting projects and holidays.

Starting with the first challenge, I was happy enough to have it flip quickly based on the behavior of the remote: press button -> get color change. However, I noticed when looking at the decoded signals across the remote that there were gaps between the values of the commands sent. For example, the decoded signal for RED (`0x036400`) and the one for OFF (`0x036403`) are very close together but there's nothing in between that the remote generates. I was hoping that those in-between values were other commands for the LED strip controller, including hopefully some other colors than the built-in remote had.

I simply wrote a quick script to generate the Flipper Zero's `.sub` files for all the values between the first value the remote sent (`0x036400`) and the last (`0x03649C`). I then just used the Flipper to send the codes one-by-one to see if anything occured from any of the non-remote codes. Nothing.

While doing this, I did notice something interesting: No matter what color I had the LED strip on, if I pressed the FADE button, it started fading from that color to RED, and then the color wheel went to YELLOW and then GREEN. This gave me an idea:

> If I set the color to GREEN, and press the FADE button, will it roll through GREEN -> RED -> GREEN?

Turns out, the answer is YES! It does touch YELLOW in between in both directions, but that just looks like more of the color cross-fading between red and green, so that's kinda neat. So my new plan was to use this oddity of behavior to get a smooth fade between GREEN and RED and back without ever telling it to switch to RED.

So I updated the code to emulate the GREEN and FADE buttons, and played with the timing between button press emulation to allow it to cross-fade from GREEN through RED and back to GREEN. Then I'd press the GREEN button again, and then FADE again, which starts fading through RED... and repeat forever.

```cpp
{% include_relative {{ page.content-path }}/testing_color_fade_raw.ino %}
```

<hr />
## 12 - Finally Encoding Original Recordings {{ badge_good_guess }} ##

The fix above didn't address the encoding issue, so I sat down and played with a hex/binary converter and tried to figure out how the decoded signals resulted in the RAW signals I was currently seeing. I first took my array for Orange (just because I had been staring at it so long), and split it up.

```cpp
Orange:
   HEX: 0x0003648A
BINARY: 0b00000000000000110110010010001010
 ARRAY: {1, -3, 1, -3, 1, -3, 1, -3, 1, -3, 1, -3, 3, -1, 3, -1,
         1, -3, 3, -1, 3, -1, 1, -3, 1, -3, 3, -1, 1, -3, 1, -3,
         3, -1, 1, -3, 1, -3, 1, -3, 3, -1, 1, -3, 3, -1, 1, -3, 1, -40}
```

I then decided to try to rearrange the array and see if I could make it more readable or reflective of the binary data. I basically was assuming here that somehow this became raw binary data with some sort of protocol overlay. I re-arranged the raw signal vertically to lose the negatives and to reflect the HIGH/LOW signal a little more visually.

```cpp
    H  1 1 1 1 1 1 3 3   1 3 3 1 1 3 1 1   3 1 1 1 3 1 3 1 1
    L...3 3 3 3 3 3 1 1   3 1 1 3 3 1 3 3   1 3 3 3 1 3 1 3 40....
```

This made me notice something interesting. First to note was that I only had two pairs of segment types (ignoring the trailing 40 segment, as it's just the end of the signal). The segments were either `{1, -3}` or `{3, -1}`. I also had a 32-bit hex number provided by Flipper, but it said `Bit: 24` in the decoded file saved for the letter. The signal overall was a 50-segment array, or more specifically, a 48 segment array + the closing on/off segment. 24 bits --> 48 segments = maybe two segments per bit?  Since I saw only the two pairings, I assumed that the `{1, -3}` was a binary `0` and `{3, -1}` was a binary `1`. This setup the following translation:

```cpp
    H  1 1 1 1 1 1 3 3   1 3 3 1 1 3 1 1   3 1 1 1 3 1 3 1 1
    L...3 3 3 3 3 3 1 1   3 1 1 3 3 1 3 3   1 3 3 3 1 3 1 3 40....
RAWBIN 0 0 0 0 0 0 1 1   0 1 1 0 0 1 0 0   1 0 0 0 1 0 1 0 0 
```

And would you look at that? I saw something very interesting there:

```cpp
BINARY: 0b00000000000000110110010010001010
RAWBIN:           0000001101100100100010100 
```
And there my endcoded signal was, matching my binary representation of my hex code.

So to handle any of my remote buttons by feeding in the hex code, I need to convert it to binary, and then send either a 3x HIGH and a 1x LOW signal for a binary `1`, or send a 1x HIGH and a 3x LOW signal for a binary `0`.

This gave me what I needed. I replicated this check a couple times with the GREEN and FADE recordings, and the logic worked out. Awesome! So I sat down and updated my code to note only reflect the Hex Code -> Binary -> raw signal conversion, but include all the rest of my remote buttons for future usage.

```cpp
{% include_relative {{ page.content-path}}/patio_lights.ino %}
```

<hr />
## 13 - Post-project follow-ups {{ badge_info }} ##

Now that I kinda understood what was going on under-the-hood of the Flipper Princeton protocol, I was better able to see how I could have poked at the firmware for Flipper and gotten to my decoding result directly.

If I looked at the `subghz_protocol_encoder_princeton_get_upload(...)` method at the firmware file [here](https://github.com/flipperdevices/flipperzero-firmware/blob/72dc8e95d0941849929af98fd51a24590d61ca31/lib/subghz/protocols/princeton.c#L109), I could see that it's doing the `{3, -1}` and `{1, -3}` style behavior. I'm not a C expert, so looking at this code gives me headaches, but it clearly has the `te * 3` functionality in that method. Understanding how Flipper's Princeton protocol setup is defined gives me the ability to look in the firmware's other protocols and find the other `subghz_protocol_encoder_PROTOCOLNAME_get_upload(...)` functions to be able to shortcut decoded signals of other types from the Flipper in the future. There'll still be some trial & error, but I have a better starting point.
