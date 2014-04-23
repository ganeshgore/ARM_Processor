import Tkinter as tk
from PIL import Image, ImageTk

class SampleApp(tk.Tk):
    '''Illustrate how to drag items on a Tkinter canvas'''

    def __init__(self, *args, **kwargs):
        tk.Tk.__init__(self, *args, **kwargs)
        self.title('ARM PRocessor Design')
        self.image1 = ImageTk.PhotoImage(Image.open("ARM.png"))
        self.panel1 = tk.Label(self, image=self.image1)
        self.panel1.pack(side='top', fill='both', expand='yes')
        self.panel1.image = self.image1

if __name__ == "__main__":
    app = SampleApp()
    app.mainloop()