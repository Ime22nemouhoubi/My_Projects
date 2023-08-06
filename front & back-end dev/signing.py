from tkinter import *
from PIL import ImageTk

#Functionality part
def signup_page():
    login_window.destroy()
    import signup
def hide():
    openeye.config(file='closeeye.png')
    passwordEntry.config(show='*')
    eyeButton.config(command=show)

def show():
    openeye.config(file='openeye.png')
    passwordEntry.config(show='')
    eyeButton.config(command=hide)

def user_enter(event):
    if usernameEntry.get()=='Username':
        usernameEntry.delete(0,END)

def password_enter(event):
    if passwordEntry.get()=='Password':
        passwordEntry.delete(0,END)
        return

#GUI PART
login_window=Tk()
login_window.resizable(None,None)
login_window.title('Login Page')
bgImage=ImageTk.PhotoImage(file='bg.jpeg')
bgLabel=Label(login_window,image=bgImage)
bgLabel.grid(row=0,column=0)

heading=Label(login_window,text='USER LOGIN',font=('Microsoft Yahei UI Light',23,'bold'),bg='white',fg='white1')
heading.place(x=660,y=120)
usernameEntry=Entry(login_window,width=25,font=('Microsoft Yahei UI Light',11,'bold'),bd=0,fg='white1')
usernameEntry.place(x=580,y=200)
usernameEntry.insert(0,'Username')
usernameEntry.bind('<FocusIn>',user_enter)
Frame(login_window,width=250,height=2,bg='white1').place(x=580,y=222)
passwordEntry=Entry(login_window,width=25,font=('Microsoft Yahei UI Light',11,'bold'),bd=0,fg='white')
passwordEntry.place(x=580,y=260)
passwordEntry.insert(0,'Password')
passwordEntry.bind('<FocusIn>',password_enter)

frame2=Frame(login_window,width=250,height=2,bg='white1')
frame2.place(x=580,y=282)

openeye=PhotoImage(file='openeye.png')
eyeButton=Button(login_window,image=openeye,bd=0,bg='white',activebackground='white',cursor='hand2',command=hide)
eyeButton.place(x=800,y=255)

forgetButton=Button(login_window,text='Forgot Password?',bd=0,bg='white',activebackground='white',cursor='hand2',font=('Microsoft Yahei UI Light',9,'bold'),fg='white1',activeforeground='white1')
forgetButton.place(x=715,y=295)

loginButton=Button(login_window,text='Login',font=('Open Sans',16,'bold'),fg='white',bg='white1',activeforeground='white',activebackground='white1',cursor='hand2',bd=0,width=19)
loginButton.place(x=578,y=350)

orLabel=Label(login_window,text='------------ OR -----------',font=('Open Sans',16),fg='white1',bg='white')
orLabel.place(x=583,y=400)
newaccountbutton=Button(login_window,text='Create new one',font=('Open Sans',9,'bold underline'),fg='blue',bg='white',activeforeground='blue',activebackground='white',cursor='hand2',bd=0,command=signup_page)




login_window.mainloop()
