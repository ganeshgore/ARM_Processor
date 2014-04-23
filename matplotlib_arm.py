from __future__ import print_function
import matplotlib.pyplot as plt
import matplotlib.image as mpimg
import numpy as np
from pylab import *
import re

def reload():
    global var_arr
    global val_arr
    var_arr = []
    val_arr = []

    with open('data.dat') as f:
        for line in f:
            if "{" in line:
                var_arr = []
                var_count = 0
                for splits in re.split("\t",line):
                    if "{" in splits:
                        var_count = var_count + 1
                        var_arr.append(re.findall("\w+",splits)[0])
                        val_arr.append(re.findall("\w+",splits)[1])
def press(event):
    global timestamp
    print('KeyPressed', event.key)
    key_press = False
    if event.key=='a':
        timestamp = timestamp - 1  if (timestamp>0) else 0
        key_press = True
    if event.key=='d':
        timestamp = timestamp + 1 if (timestamp<(len(val_arr)/var_count)-1)  else timestamp
        key_press = True
    if event.key == 'r':
        reload()
        timestamp = 0;
        key_press = True

    txt = ""
    if key_press:
        xl.set_text("CurrentTimeStamp - %d"%(timestamp))
        for i in range(0,var_count):
            # lbl_arr[i].set_text(val_arr[(var_count*timestamp)+i])
            if (lbl_arr[i].get_text() == val_arr[(var_count*timestamp)+i]):
                lbl_arr[i].set_text(val_arr[(var_count*timestamp)+i])
                lbl_arr[i].set_alpha(0.5)
                lbl_arr[i].set_fontsize(13)
            else:
                lbl_arr[i].set_text(val_arr[(var_count*timestamp)+i])
                lbl_arr[i].set_alpha(1.0)
                lbl_arr[i].set_fontsize(17)
            txt = txt + str(val_arr[(var_count*timestamp)+i]) + "|"
        # x2.set_text(txt)
        fig.canvas.draw()



class Cursor:
    def __init__(self, ax):
        self.ax = ax
        self.lx = ax.axhline(color='k')  # the horiz line
        self.ly = ax.axvline(color='k')  # the vert line

        # text location in axes coords
        self.txt = ax.text( 0.01, 0.90, '', transform=ax.transAxes)

    def mouse_move(self, event):
        if not event.inaxes: return

        x, y = event.xdata, event.ydata
        # update the line positions
        self.lx.set_ydata(y)
        self.ly.set_xdata(x)

        self.txt.set_text( 'x=%1.2f, y=%1.2f'%(x/xpixels,1-(y/ypixels)) )
        draw()


img = mpimg.imread('ARM.png')
ypixels, xpixels, bands = img.shape
print (ypixels, xpixels, bands)
fig, ax = subplots()
plt.imshow(img)
cursor = Cursor(ax)
connect('motion_notify_event', cursor.mouse_move)
var_arr = []
val_arr = []

with open('data.dat') as f:
    for line in f:
        if "{" in line:
            var_arr = []
            var_count = 0
            for splits in re.split("\t",line):
                if "{" in splits:
                    var_count = var_count + 1
                    var_arr.append(re.findall("\w+",splits)[0])
                    val_arr.append(re.findall("\w+",splits)[1])

var_lst  = []
xval = []
yval = []
with open("loc.dat", 'r') as f:
    for line in f:
        items = line.split()
        print (items)
        var_lst.append(items[0])
        xval.append(items[1])
        yval.append(items[2])

timestamp = 0
lbl_arr = [None] * var_count
count = 0
for val in var_arr:
    if val in var_lst:
        ind = var_lst.index(val)
        lbl1 = ax.text(xval[ind],yval[ind],val,fontsize=15,transform=ax.transAxes,color="blue",weight="bold",bbox=dict(facecolor='yellow', alpha=0.6))
        lbl_arr[count] =lbl1
        count = count + 1
        print ("Found " , val  )
    else:
        lbl1 = ax.text(0.01,0.1,val,fontsize=15,transform=ax.transAxes,color="blue",weight="bold",bbox=dict(facecolor='yellow', alpha=0.6))
        print ("NOT Found " , val  )
        lbl_arr[count] = lbl1
        count = count + 1

fig.canvas.mpl_connect('key_press_event', press)
xl = ax.text(0.01, 0.95,("CurrentTimeStamp - %d"%(timestamp)),fontsize=20,transform=ax.transAxes)
# x2 = ax.text(0.01, 0.01,("Line - %d"%(timestamp)),fontsize=15,transform=ax.transAxes)
plt.show()
