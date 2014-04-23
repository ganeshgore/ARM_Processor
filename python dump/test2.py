# import PIL
# from PIL import ImageFont
# from PIL import Image
# from PIL import ImageDraw
# font = ImageFont.truetype("Senats-Antiqua.ttf",25)
# img=Image.new("RGBA", (200,200),(120,100,20))
# draw = ImageDraw.Draw(img)
# draw.text((0, 0),"This is a test",(255,255,0),font=font)
# draw = ImageDraw.Draw(img)
# draw = ImageDraw.Draw(img)
# img.save("a_test.png")

#!/usr/bin/env python
# -*- noplot -*-

"""

This example shows how to use matplotlib to provide a data cursor.  It
uses matplotlib to draw the cursor and may be a slow since this
requires redrawing the figure with every mouse move.

Faster cursoring is possible using native GUI drawing, as in
wxcursor_demo.py
"""
# from __future__ import print_function
# from pylab import *


# class Cursor:
    # def __init__(self, ax):
        # self.ax = ax
        # self.lx = ax.axhline(color='k')  # the horiz line
        # self.ly = ax.axvline(color='k')  # the vert line

        # # text location in axes coords
        # self.txt = ax.text( 0.7, 0.9, '', transform=ax.transAxes)

    # def mouse_move(self, event):
        # if not event.inaxes: return

        # x, y = event.xdata, event.ydata
        # # update the line positions
        # self.lx.set_ydata(y )
        # self.ly.set_xdata(x )

        # self.txt.set_text( 'x=%1.2f, y=%1.2f'%(x,y) )
        # draw()


# class SnaptoCursor:
    # """
    # Like Cursor but the crosshair snaps to the nearest x,y point
    # For simplicity, I'm assuming x is sorted
    # """
    # def __init__(self, ax, x, y):
        # self.ax = ax
        # self.lx = ax.axhline(color='k')  # the horiz line
        # self.ly = ax.axvline(color='k')  # the vert line
        # self.x = x
        # self.y = y
        # # text location in axes coords
        # self.txt = ax.text( 0.7, 0.9, '', transform=ax.transAxes)

    # def mouse_move(self, event):

        # if not event.inaxes: return

        # x, y = event.xdata, event.ydata

        # indx = searchsorted(self.x, [x])[0]
        # x = self.x[indx]
        # y = self.y[indx]
        # # update the line positions
        # self.lx.set_ydata(y )
        # self.ly.set_xdata(x )

        # self.txt.set_text( 'x=%1.2f, y=%1.2f'%(x,y) )
        # print ('x=%1.2f, y=%1.2f'%(x,y))
        # draw()

# t = arange(0.0, 1.0, 0.01)
# s = sin(2*2*pi*t)
# fig, ax = subplots()

# cursor = Cursor(ax)
# #cursor = SnaptoCursor(ax, t, s)
# connect('motion_notify_event', cursor.mouse_move)

# ax.plot(t, s, 'o')
# axis([0,1,-1,1])
# show()



#!/usr/bin/env python

"""
Show how to connect to keypress events
"""
# from __future__ import print_function
import sys
import numpy as np
import matplotlib.pyplot as plt


def press(event):
    print('press', event.key)
    sys.stdout.flush()
    if event.key=='x':
        visible = xl.get_visible()
        xl.set_visible(not visible)
        fig.canvas.draw()

fig, ax = plt.subplots()

fig.canvas.mpl_connect('key_press_event', press)

ax.plot(np.random.rand(12), np.random.rand(12), 'go')
xl = ax.set_xlabel('easy come, easy go')

plt.show()