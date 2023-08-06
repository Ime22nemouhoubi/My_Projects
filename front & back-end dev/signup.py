from tkinter import *
from PIL import ImageTk

def login_page():
    signup_window.destroy()
    import signing
signup_window=Tk()
signup_window.title('Signup Page')
signup_window.resizable(False,False)
background=ImageTk.PhotoImage(file='bg.jpeg')

bgLabel=Label(signup_window,image=background)
bgLabel.grid()

frame=Frame(signup_window,bg='white')
frame.place(x=554,y=100)
heading=Label(frame,text='CREATE AN ACCOUNT',font=('Microsoft Yahei UI Light',23,'bold'),bg='white',fg='white1')
heading.grid(row=0,column=0,padx=10,pady=10)
emaillabel=Label(frame,text='Email',font=('Microsoft Yahei UI Light',10,'bold'),bg='white',fg='white1')
emaillabel.grid(row=1,column=0,sticky='w',padx=25,pady=(10,0))
emailEntry=Entry(frame,width=30,font=('Microsoft Yahei UI Light',10,'bold'),fg='white',bg='white1')
emailEntry.grid(row=2,column=0,sticky='w',padx=25)

usernamelabel=Label(frame,text='User Name',font=('Microsoft Yahei UI Light',10,'bold'),bg='white',fg='white1')
usernamelabel.grid(row=3,column=0,sticky='w',padx=25,pady=(10,0))
usernameEntry=Entry(frame,width=30,font=('Microsoft Yahei UI Light',10,'bold'),fg='white',bg='white1')
usernameEntry.grid(row=4,column=0,sticky='w',padx=25)

passwordlabel=Label(frame,text='Password',font=('Microsoft Yahei UI Light',10,'bold'),bg='white',fg='white1')
passwordlabel.grid(row=5,column=0,sticky='w',padx=25,pady=(10,0))
passwordEntry=Entry(frame,width=30,font=('Microsoft Yahei UI Light',10,'bold'),fg='white',bg='white1')
passwordEntry.grid(row=6,column=0,sticky='w',padx=25)

confirmlabel=Label(frame,text='Confirm Password',font=('Microsoft Yahei UI Light',10,'bold'),bg='white',fg='white1')
confirmlabel.grid(row=7,column=0,sticky='w',padx=25,pady=(10,0))
confirmEntry=Entry(frame,width=30,font=('Microsoft Yahei UI Light',10,'bold'),fg='white',bg='white1')
confirmEntry.grid(row=8,column=0,sticky='w',padx=25)

termdandconditions=Checkbutton(frame,text='I agree to the Terms & conditions',font=('Open Sans',10,'bold'),fg='white1',bg='white',activebackground='white',activeforeground='white1',cursor='hand2')
termdandconditions.grid(row=9,column=0,pady=10,padx=15)

signupButton=Button(frame,text='Signup',font=('Microsoft Yahei UI Light',10,'bold'),bd=0,bg='white1',fg='white',activebackground='white1',activeforeground='white',width=17)
signupButton.grid(row=10,column=0,pady=10)
alreadyhaveanaccount=Label(frame,text="Don't have an account?",font=('Open Sans','9','bold'),bg='white',fg='white1')
alreadyhaveanaccount.grid(row=11,column=0,sticky='w',padx=25,pady=10)

loginbutton=Button(frame,text='Log in',font=('Open Sans','9','bold underline'),bg='white',fg='blue',bd=0,cursor='hand2',activeforeground='blue',activebackground='white',command=login_page)
loginbutton.place(x=170,y=404)





signup_window.mainloop()