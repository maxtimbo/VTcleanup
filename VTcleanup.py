import tkinter as tk
import tkinter.filedialog
from tkinter import ttk
from tkinter import messagebox
from tkinter import simpledialog
import os, shutil, datetime, time
import configparser
import threading
        
class gloVars:
    cnf = configparser.ConfigParser()
    cnfExist = False
    rootDir = 'C:/RemoteVT'
    station = '' 
    cart = ''
    fileList = {}
    fileSelection = []
    exportDir = ''
    todayFlag = 0
    todayTest = False
    today = datetime.date.today().strftime('%m/%d/%Y')
    seconds = 0

    @classmethod
    def updateDict(self):
        try:
            fullpath = f'{self.station}/AUDIO/{self.cart}'
            fileList = os.listdir(fullpath)
            mtime = lambda f: os.stat(os.path.join(fullpath, f)).st_mtime
            fileList.sort(key=mtime)
            timeList = []
            for x in fileList:
                y = f'{fullpath}/{x}'
                if not os.path.isdir(y):
                    fileModTime = os.path.getmtime(y)
                    modTime = datetime.datetime.fromtimestamp(fileModTime).strftime('%m/%d/%Y')
                    timeList.append(modTime)
                    if modTime not in gloVars.fileList:
                        self.fileList.setdefault(modTime, []).append(x)
                    else:
                        self.fileList[modTime].append(x)
        except:
            gloVars.fileList = {}


    @classmethod
    def add_to_selection(cls, value):
        cls.fileSelection.append(value)

    def __init__(self, value):
        self.value = value
        self.add_to_selection(value)

class Step1(ttk.Frame):
## Station and Cart Select
    def __init__(self, parent, *args, **kwargs):
        ttk.Frame.__init__(self, parent, *args, **kwargs)
        
        leftFrame = ttk.Frame(self)
        leftFrame.pack(side="left", fill="x", expand=True)
        rightFrame = ttk.Frame(self)
        rightFrame.pack(side="right", fill="x", expand=True)
        
        self.stationSel = tk.StringVar()
        self.cartSel = tk.StringVar()
        self.cartDir = tk.StringVar()

        self.stationlbl = ttk.Label(leftFrame, text="Select Station: ").pack()
        self.stationListDisp = tk.Listbox(leftFrame, exportselection=False)
        self.stationListDisp.pack()
        self.stationListDisp.bind("<<ListboxSelect>>", self.setStation)

        self.cartlbl = ttk.Label(rightFrame, text="Select Cart: ").pack()
        self.cartListDisp = tk.Listbox(rightFrame, exportselection=False)
        self.cartListDisp.pack()
        self.cartListDisp.bind("<<ListboxSelect>>", self.setCart)
        self.checkGlo()
    
    def checkGlo(self):
        self.stationListDisp.delete(0, tk.END)
        for x in os.listdir(f'{gloVars.rootDir}/VTVI'):
            self.stationListDisp.insert(tk.END, x)

        if gloVars.cnfExist:
            self.stationSel.set(gloVars.station)
            for x in range(self.stationListDisp.size()):
                if self.stationListDisp.get(x) == os.path.basename(gloVars.station):
                    self.stationListDisp.selection_set(x)

            self.cartListDisp.delete(0, tk.END)
            for x in os.listdir(f'{self.stationSel.get()}/AUDIO/'):
                self.cartListDisp.insert(tk.END, x)

            for x in range(self.cartListDisp.size()):
                if self.cartListDisp.get(x) == gloVars.cart:
                    self.cartListDisp.selection_set(x)

    def setCart(self, e):
        gloVars.cart = ''
        k = self.cartListDisp.curselection()
        if k != ():
            y = self.cartListDisp.curselection()[0]
            self.cartDir.set(f'{self.stationSel.get()}/AUDIO/{self.cartListDisp.get(y)}')
            self.cartSel.set(self.cartListDisp.get(y))
            gloVars.cart = self.cartSel.get()

        gloVars.updateDict()
    
    def setStation(self, e = None):
        gloVars.station = ''
        k = self.stationListDisp.curselection()
        if k != ():
            y = self.stationListDisp.curselection()[0]
            self.cartListDisp.delete(0, tk.END)
            station = f'{gloVars.rootDir}/VTVI/{self.stationListDisp.get(y)}'
            try:
                for x in os.listdir(f'{station}/AUDIO'):
                    self.cartListDisp.insert(tk.END, x)
            except:
                self.cartListDisp.insert(tk.END, "Invalid Selection")
                station = "invalid selection"
            gloVars.station = station
            #Step3.refresh_()

class Step3(ttk.Frame):
## Dates and file selection screen
    def __init__(self, parent, *args, **kwargs):
        ttk.Frame.__init__(self, parent, *args, **kwargs)
        self.parent = parent
        self.seconds =0

        self.stationVar = tk.StringVar()
        self.cartVar = tk.StringVar()
        header = ttk.Label(self, text="Select date (based on modification date): ")
        header.pack(side="top", fill="both", expand=True)
       
        leftPane = ttk.Frame(self)
        leftPane.pack(side="left", fill="both", expand=True)
        rightPane = ttk.Frame(self)
        rightPane.pack(side="right", fill="both", expand=True)
        stationlbl = ttk.Label(leftPane, text="Station Selected: ").pack()
        self.station = ttk.Label(leftPane, textvariable=self.stationVar)
        self.station.pack()

        cartlbl = ttk.Label(rightPane, text="Cart Selected: ").pack()
        self.cart = ttk.Label(rightPane, textvariable=self.cartVar)
        self.cart.pack()

        dates_lbl = ttk.Label(leftPane, text="Dates available: ").pack()
        self.dates_list = tk.Listbox(leftPane, exportselection=False)
        self.dates_list.pack()
        self.dates_list.bind("<<ListboxSelect>>", self.date_file_selection)

        files_lbl = ttk.Label(rightPane, text="Files available: ").pack()
        self.files_list = tk.Listbox(rightPane, exportselection=False, selectmode="multiple")
        self.files_list.pack()
        self.files_list.bind("<<ListboxSelect>>", self.set_gloVars)

        self.station.after(1000, self.refresh_label)

    @classmethod
    def refresh_(cls):
        cls.refresh_label(cls)
    
    def refresh_label(self):
        if gloVars.station != self.stationVar.get():
            if gloVars.cart != self.cartVar.get():
                self.files_list.delete(0, tk.END)
                self.dates_list.delete(0, tk.END)
                self.stationVar.set(os.path.basename(gloVars.station))
                self.cartVar.set(gloVars.cart)
                for x in gloVars.fileList.keys():
                    self.dates_list.insert(tk.END, x)
                    for y in gloVars.fileList[x]:
                        self.files_list.insert(tk.END, y)
                if gloVars.todayFlag == 1:
                    for x in range(self.dates_list.size()):
                        if self.dates_list.get(x) == gloVars.today:
                            self.dates_list.selection_set(x)
                    if not gloVars.todayTest:
                        messagebox.showwarning(title="Today not present", message="""Files modified today are not present.\nThis likely means that either you've not voice tracked today, or something else went wrong.\nYou can add other dates and files here.""")
                    else:
                        fileValues = gloVars.fileList[gloVars.today]
                        for file_list_iter in range(self.files_list.size()):
                            for file_value in fileValues:
                                if self.files_list.get(file_list_iter) == file_value:
                                    self.files_list.selection_set(file_list_iter)
                        self.set_gloVars()

        self.seconds += 1
        self.station.after(1000, self.refresh_label)

    def date_file_selection(self, e):
        self.files_list.selection_clear(0, tk.END)
        date_selection_index = self.dates_list.curselection()
        if date_selection_index != ():
            date_selection_name = self.dates_list.get(date_selection_index)
            fileValues = gloVars.fileList[date_selection_name]
            for file_list_iter in range(self.files_list.size()):
                for file_value in fileValues:
                    if self.files_list.get(file_list_iter) == file_value:
                        self.files_list.selection_set(file_list_iter)
        self.set_gloVars()

    def set_gloVars(self, e = None):
        gloVars.fileSelection.clear()
        for i in range(self.files_list.size()):
            if self.files_list.selection_includes(i):
                gloVars.add_to_selection(self.files_list.get(i))
            else:
                pass
                       
class Step2(ttk.Frame):
## Export Folder, set 'today' flag, save and display cnf
    def __init__(self, parent, *args, **kwargs):
        ttk.Frame.__init__(self, parent, *args, **kwargs)
        self.parent = parent
        self.exportDir = tk.StringVar()
        self.todayVar = tk.IntVar()
        self.checkGlos()

        head_btns = ttk.Frame(self)
        head_btns.pack(side="top", fill="both", expand=True)
        foot = ttk.Frame(self)
        foot.pack(side="top", fill="both", expand=True)
        header = ttk.Label(head_btns, text="Select Export Folder: ")
        header.pack(side="top", fill="both", expand=True)
       
        exportDisplay = ttk.Entry(head_btns, textvariable=self.exportDir)
        exportDisplay.pack(side="left", fill="x", expand=True)

        exportSelbtn = ttk.Button(head_btns, text=". . .", command=self.exportSel_click)
        exportSelbtn.pack(side="right", fill="x")

        cnfNotes = ttk.Label(foot, text="""
        You can save the configuration to always run the selected station and cart.
        You can also choose to always find files modified \'today\'""")
        cnfNotes.pack()

        self.todayFlag = ttk.Checkbutton(foot, text="Set \'Today\' Flag", variable=self.todayVar, command=self.updateGlo)
        self.todayFlag.pack()
        self.todayFlag.todayVar = self.todayVar

        self.savebtn = ttk.Button(foot, text="Save Config", command=self.clickSave)
        self.savebtn.pack(side="right")

    def updateGlo(self):
        gloVars.todayFlag = self.todayVar.get()

    def exportSel_click(self):
        exDir = tk.filedialog.askdirectory(title="Select Export Directory")
        if os.path.isdir(exDir):
            self.exportDir.set(exDir)
            gloVars.exportDir = exDir
        else:
            pass

    def checkGlos(self):
        if gloVars.cnfExist:
            self.exportDir.set(gloVars.exportDir)
            if gloVars.todayFlag == 1:
                self.todayVar.set(1)
            else:
                self.todayVar.set(0)

    def clickSave(self):
        ini_file = f'{gloVars.rootDir}/vt_export_cnf.ini'
        with open(ini_file, 'w') as f:
            f.write(f'[{os.path.basename(gloVars.station)}]\n')
            f.write(f'cart = {gloVars.cart}\n')
            f.write(f'fullpath = \"{gloVars.station}/AUDIO/{gloVars.cart}\"\n')
            f.write(f'today = {gloVars.todayFlag}\n')
            f.write(f'exportpath = \"{gloVars.exportDir}\"\n')
        self.savebtn.config(text="done", state="disabled")

class Step4(ttk.Frame):
## Show final command and run
    def __init__(self, parent, *args, **kwargs):
        ttk.Frame.__init__(self, parent, *args, **kwargs)
        self.parent = parent
        self.seconds = 0
        self.var = tk.StringVar()
        self.show_final = tk.Message(self, textvariable=self.var, anchor="nw", width="400")
        #self.show_final = tk.Text(self)
        self.show_final.pack()
        self.show_final.after(1000, self.populate)
        #self.populate(self.show_final)
    
    def populate(self):
        files = {}
        s = '\n'
        g = f'{gloVars.station}/AUDIO/{gloVars.cart}'
        for x in gloVars.fileSelection:
            k = x.replace("SP", gloVars.cart)
            files[f'From: {g}/{x}'] = f'\tTo:  {gloVars.exportDir}/{k}'
        self.var.set(f'Files to be copied:\n{s.join(c for kv in files.items() for c in kv)}')
        self.seconds += 1
        self.show_final.after(1000, self.populate)

class fini:
    def __init__(self):
        self.__init__(self)

    @classmethod
    def progress(cls, dictionary):
        src = dictionary.keys()
        dst = dictionary.values()
        popup = tk.Toplevel()
        lbl = ttk.Label(popup, text="Uploading Files").pack()
        progress_bar = ttk.Progressbar(popup)
        progress_bar.pack(fill="x", expand=True, padx=20, pady=20)
        progress_bar.start()
        def operation(src, dest):
            threads = []
            #labels = []
            #progs = []
            for x, y in zip(src, dest):
                #labels.append(ttk.Label(popup, text=f'Copying {x}\nTo:{y}'))
                #labels[-1].pack()
                #progs.append(ttk.Progressbar(popup))
                #progs[-1].pack(expand=True, fill="x", padx=20)
                #progs[-1].start()
                threads.append(threading.Thread(target=cls._copy, args=(x, y)))
                threads[-1].start()

            for t in threads:
                t.join()

            popup.destroy()
        runThread = threading.Thread(target=operation, args=(src, dst))
        runThread.start()

    def _copy(src, dest):
        ### Need to write better checks here in case things go wrong during the upload.
        fullsrc = f'{gloVars.station}/AUDIO/{gloVars.cart}/{src}'
        fulldest = f'{gloVars.exportDir}/{dest}'
        try:
            shutil.copy2(fullsrc, fulldest)
        except Exception as e:
            messagebox.showerror(title="Something Went Wrong", message=f'The file {src} did not upload correctly. Check your connection first.\n{e}')

class quickSetup(tk.Toplevel):
    x = 0
    def __init__(self, parent, *args, **kwargs):
        ttk.Frame.__init__(self, parent, *args, **kwargs)
        self.parent = parent
        self.current_step = None
        self.init_check_root()
        self.steps = [Step1(parent), Step2(parent), Step3(parent), Step4(parent)]

        self.content_frame = ttk.Frame(parent)
        self.content_frame.pack(fill="x")
        self.button_frame = ttk.Frame(parent)
        self.button_frame.pack(side="bottom", fill="x", padx=10, pady=10)
        
        self.back_btn = ttk.Button(self.button_frame, text="<< Back", command=self.clickBack)
        self.next_btn = ttk.Button(self.button_frame, text="Next >>", command=self.clickNext)
        self.fini_btn = ttk.Button(self.button_frame, text="Finish", command=self.clickFini)

        self.stop_thread = threading.Event()

        if gloVars.cnfExist:
            self.x += 2
            if gloVars.todayFlag == 1:
                self.x += 1 
                if not gloVars.todayTest:
                    self.x -= 1 
        self.show_step(self.x)

    def show_step(self, step):
        if self.current_step is not None:
            current_step = self.steps[self.current_step]
            current_step.pack_forget()
    
        self.current_step = step
    
        new_step = self.steps[step]
        new_step.pack(fill="both", expand=True, padx=10, pady=10)
    
        if step == 0:
            self.back_btn.pack_forget()
            self.next_btn.pack(side="right")
            self.fini_btn.pack_forget()

        elif step == len(self.steps)-1:
            self.back_btn.pack(side="left")
            self.next_btn.pack_forget()
            self.fini_btn.pack(side="right")

        else:
            self.back_btn.pack(side="left")
            self.next_btn.pack(side="right")
            self.fini_btn.pack_forget()
    
    def clickBack(self):
        self.show_step(self.current_step - 1)

    def clickNext(self):
        self.show_step(self.current_step + 1)

    def clickFini(self):
        fileDict = {}
        for x in gloVars.fileSelection:
            k = x.replace("SP", gloVars.cart)
            fileDict[x] = k
        fin = fini
        fin.progress(fileDict)
        #popup = tk.Toplevel()
        #albl = ttk.Label(popup, text="Files are being uploaded").pack()
        #progress = 0
        #progress_var = tk.DoubleVar()
        #progress_bar = ttk.Progressbar(popup, variable=progress_var, maximum=100)
        #progress_bar.pack()
        #files = {}
        #g = f'{gloVars.station}/AUDIO/{gloVars.cart}'
        #progress_bar.start()
        #for x in gloVars.fileSelection:
        #    k = x.replace("SP", gloVars.cart)
        #    files[f'{g}/{x}'] = f'{gloVars.exportDir}/{k}'
        #k = files.keys()
        #v = files.values()
        ### Putting a pin in it tonight. Need to use concurrent.futures rather than threading
        ### make operation() require two arguments and only do one thing at a time.
        #def operation():

        #    time.sleep(1)
        #    if messagebox.showinfo(title="Done", message="yeah") == "ok":
        #        root.destroy()

        #    #try:
        #    #    for x in files:
        #    #        time.sleep(1)
        #    #       #shutil.copy2(x, files[x])
        #    #    if messagebox.showinfo(title="Done", message="Operation Successful!") == "ok":
        #    #        stop_threading.set()
        #    #        root.destroy()
        #    #except:
        #    #    messagebox.showerror(title="Error", message="Something went wrong.\nCheck your settings and connections.")
        #    progress_bar.stop()
        #    return 0
        #threading.Thread(target=operation).start()

    def init_check_root(self):
        if not os.path.isdir(gloVars.rootDir):
            if messagebox.askyesno(title="Could not find RemoteVT", message="""Could not find C:\\RemoteVT. This could mean that RemoteVT software is not installed. Or it means that RemoteVT is installed in a non-standard location. Would you like to specify a different Directory for RemoteVT?"""):
                rootDir = tk.filedialog.askdirectory(title="Select RemoteVT Root")
                gloVars.rootDir = rootDir

        if os.path.isfile(f'{gloVars.rootDir}/vt_export_cnf.ini'):
            gloVars.cnfExist = True
            gloVars.cnf.read(f'{gloVars.rootDir}/vt_export_cnf.ini')
            if len(gloVars.cnf.sections()) == 1:
                g = gloVars.cnf.sections()
                gloVars.station = f'{gloVars.rootDir}/VTVI/{g[0]}'
            else:
                print("Theres more than one")
            gloVars.cart = gloVars.cnf[os.path.basename(gloVars.station)]['cart']
            gloVars.exportDir = gloVars.cnf[os.path.basename(gloVars.station)]['exportpath']
            f = gloVars.exportDir.replace("\"", "")
            gloVars.exportDir = f
            gloVars.todayFlag = int(gloVars.cnf[os.path.basename(gloVars.station)]['today'])
            gloVars.updateDict()

            for x in gloVars.fileList.keys():
                if x == gloVars.today:
                    gloVars.todayTest = True



if __name__ == "__main__":
    root = tk.Tk()
    quickSetup(root)
    root.wm_title("VT Emergency Export")
    root.bind("<Escape>", (lambda event: exit()))
    root.geometry("450x350")
    root.mainloop()
