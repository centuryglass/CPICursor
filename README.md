# CPICursor
### A WIP virtual mouse cursor for the ClockworkPi GameShell.

The ClockworkPi system does not have a touch screen or built in mouse support. CPICursor is an attempt to create a simulated mouse cursor, controlled by the ClockworkPi's directional pad and buttons.

As far as I can tell, there's no good way to do this directly through X11, so I'm bypassing it entirely. CPICursor uses secure helper daemons to read button inputs and draw the cursor. cursorKeyd reads cursor control inputs directly from system keyboard event files, and cursorPainterd draws the cursor directly to the framebuffer. These daemons use my DaemonFramework library and custom security capabilities to limit potential security issues. *TODO: ClockworkPi does not seem to support custom capabilities, all calls to `setcap` fail. This needs to be fixed to securely run helper daemons.*

Creating actual mouse click events is not yet implemented, but the current plan is to generate them using xdotool. Likewise, using a key binding to close CPICursor has not yet been implemented.

### Testing on ClockworkPi
This project is not yet functional, but current progress can be tested by connecting to your ClockworkPi over ssh and running the following commands:
1. `git clone --recursive https://github.com/centuryglass/CPICursor`
2. `cd CPICursor`
3. `make`
4. `make install`
5. If you don't have it already, install xdotool with `sudo apt-get install xdotool`
6. Run `DISPLAY=:0 xdotool key ctrl+alt+F2` to switch from X11 to tty. 
7. Run `sudo CPICursor` and use the d-pad to test moving the cursor.
8. When finished, run `systemctl restart` to restart your device, as xdotool won't work within tty.

### Current progress:
#### Desktop testing
Cursor drawing and control are both tested and working within tty on an x64 system running Arch Linux. Drawing to the framebuffer does not work when X11 is active.

#### CPI testing
Drawing to the framebuffer over X11 was previously working, but no longer functions after updating the OS to V0.4. As mentioned above, adding daemon security capabilities with `setcap` fails, so running CPICursor only works if run as root. Drawing and controlling the cursor does still function within a tty interface.

### Further research required:

- **Is there a way to directly enable and disable the mouse cursor in X11, even when no input device exists?** If this was possible, it would simplify things considerably, and I could just use xdotool to move the cursor.
- **Is it possible to create a fake pointer device that runs through software?** This would also be better than my current approach.
- **Is there a way to make X11 draw a cursor image over any window?** Direct framebuffer access is less than ideal, and it would be better if I could avoid it.
- **If not, how can I enable drawing to the framebuffer over X11 again?** In ClockworkOS V0.2, images could be drawn directly to the framebuffer even with X11 active. Is there some way this can be re-enabled?
- **How can I enable custom security capabilities?** I suspect these are disabled in the kernel, but can't be sure as the kernel configuration isn't accessible in any of the usual locations in /proc or /boot. Is there a way to enable these without installing a new kernel?
