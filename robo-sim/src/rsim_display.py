from __future__ import division 
import sys
from . import sgc
from sgc.locals import *
import pygame
from pygame.locals import *
from pgu import gui

version = "V2.2.0 Xiao Edition"

PYTHON_MAJOR = sys.version_info[0]

import os

from ConfigParser import SafeConfigParser
from src.loadscenery import SceneryParser, LoadError
from src.constants import L, R, NO_NOISE, SAMPLED_NOISE, GAUSSIAN_NOISE, ANGULAR_NOISE

from math import sqrt

class ErrorDialog(gui.Dialog):
    def __init__(self, msg):
        title = gui.Label("Load Error")

        t = gui.Table()

        t.tr()
        t.add(gui.Label(msg))

        t.tr()
        e = gui.Button("Ok", width=100)
        e.connect(gui.CLICK, self.close, None)
        t.td(e)

        gui.Dialog.__init__(self,title,t)

class PrefsDialog(gui.Dialog):
    def __init__(self, noise_model, noise_value, rand_under):
        title = gui.Label("Simulation Preferences")

        ##Once a form is created, all the widgets that are added with a name             
        ##are added to that form.                                                        
        ##::                                                                             
        self.value = gui.Form()
        self.noise = [0,0,5,5]
        self.noise[noise_model] = noise_value
        self.prev_noise_model = noise_model

        t = gui.Table()


        t.tr()
        t.td(gui.Spacer(width=8,height=8))
        t.tr()
        t.td(gui.Label("IR Sensor Noise",align=0), colspan = 2)
        t.tr()
        t.td(gui.Spacer(width=8,height=8))

        t.tr()
        g = gui.Group(name="irnoise",value=noise_model)
        tt = gui.Table()
        g.connect(gui.CHANGE, self.switch_noise, (tt,g))
        tt.tr()
        radio0 = gui.Radio(g,value=NO_NOISE)
        tt.td(radio0)
        tt.td(gui.Label(" None"),align=-1, width=300, colspan=3)
        tt.tr()
        radio2 = gui.Radio(g,value=SAMPLED_NOISE)
        tt.td(radio2)
        tt.td(gui.Label(" Sampled"),align=-1, width=300, colspan=3)
        tt.tr()
        radio3 = gui.Radio(g,value=GAUSSIAN_NOISE)
        tt.td(radio3)
        tt.td(gui.Label(" Gaussian"),align=-1)
        self.noise_slider1 = gui.HSlider(name="noise1", value=self.noise[GAUSSIAN_NOISE], 
                                         min=1, max=9, size = 20, width=80, height=16)
        self.noise_label1 = gui.Label("Noise Level " + str(self.noise_slider1.value))
        self.noise_slider1.connect(gui.CHANGE, self.adjust_noise, 
                                   (self.noise_slider1, self.noise_label1, GAUSSIAN_NOISE))
        if noise_model == GAUSSIAN_NOISE:
            tt.add(self.noise_slider1, 2, 2)
            tt.add(self.noise_label1, 3, 2)
        tt.tr()
        radio4 = gui.Radio(g,value=ANGULAR_NOISE)
        tt.td(radio4)
        tt.td(gui.Label(" Angular"),align=-1)
        self.noise_slider2 = gui.HSlider(name="noise2", value=self.noise[ANGULAR_NOISE], 
                                         min=1, max=9, size = 20, width=80, height=16)
        self.noise_label2 = gui.Label("Noise Level " + str(self.noise_slider2.value))
        self.noise_slider2.connect(gui.CHANGE, self.adjust_noise, 
                                   (self.noise_slider2, self.noise_label2, ANGULAR_NOISE))
        if noise_model == ANGULAR_NOISE:
            tt.add(self.noise_slider2,2,3)
            tt.add(self.noise_label2,3,3)
        t.td(tt,colspan=2)
        t.tr()

        tt = gui.Table()
        tt.tr()
        tt.td(gui.Spacer(width=8,height=16))
        tt.tr()
        tt.td(gui.Label("Extra Sensor Options"), colspan=2)
        tt.tr()
        tt.td(gui.Spacer(width=8,height=8))
        tt.tr()
        tt.td(gui.Switch(name="randunder", value=rand_under))
        tt.td(gui.Label(" Random under-reads")) 
        tt.tr()
        tt.td(gui.Spacer(width=8,height=8))
        tt.tr()
        t.td(tt,colspan=2)
        

        t.tr()
        t.td(gui.Spacer(width=8,height=8))

        ##The okay button CLICK event is connected to the Dialog's                       
        ##send event method.  It will send a gui.CHANGE event.                           
        ##::                                                                             
        t.tr()
        e = gui.Button("Okay")
        e.connect(gui.CLICK,self.send,gui.CHANGE)
        t.td(e)
        ##                                                                               

        e = gui.Button("Cancel")
        e.connect(gui.CLICK,self.close,None)
        t.td(e)

        gui.Dialog.__init__(self,title,t)

    def switch_noise(self, value):
        (tt, group) = value
        noise_model = group.value
        if noise_model == GAUSSIAN_NOISE:
            tt.add(self.noise_slider1, 2, 2)
            tt.add(self.noise_label1, 3, 2)
        elif noise_model == ANGULAR_NOISE:
            tt.add(self.noise_slider2, 2, 3)
            tt.add(self.noise_label2, 3, 3)
        if self.prev_noise_model == GAUSSIAN_NOISE and noise_model != GAUSSIAN_NOISE:
            tt.remove(self.noise_slider1)
            tt.remove(self.noise_label1)
        if self.prev_noise_model == ANGULAR_NOISE and noise_model != ANGULAR_NOISE:
            tt.remove(self.noise_slider2)
            tt.remove(self.noise_label2)
        self.prev_noise_model = noise_model

    def adjust_noise(self, value):
        (slider, label, num) = value
        label.set_text("Noise Level " + str(slider.value))


class colour_Prefs(gui.Dialog):
    def __init__(self, r,g,b, titleBar):
        title = gui.Label(titleBar + " Preferences")

        ##Once a form is created, all the widgets that are added with a name             
        ##are added to that form.                                                        
        ##::                                                                             
        self.value = gui.Form()

        self.r = r
        self.g = g
        self.b = b

        self.setColour = pygame.Color(self.r, self.g, self.b, 100)

        t = gui.Table()

        t.tr()
        t.td(gui.Spacer(width=8,height=8))
        t.tr()
        t.td(gui.Label("Pick a colour for " + titleBar,align=0), colspan = 2)
        t.tr()
        t.td(gui.Spacer(width=8,height=8))
        t.tr()
        tt = gui.Table()
        tt.tr()
        
        self.colour_slider1 = gui.HSlider(name="r", value=self.r, 
                                         min=0, max=255, size = 20, width=255, height=16)
        self.colour_label1 = gui.Label("Red: " + str(self.colour_slider1.value))
        self.colour_slider1.connect(gui.CHANGE, self.adjust_colour, 
                                   (self.colour_slider1, self.colour_label1, "Red"))
        tt.add(self.colour_slider1, 2, 2)
        tt.tr()
        tt.add(self.colour_label1, 2, 3)
        tt.tr()
        tt.td(gui.Spacer(width=8,height=8))
        self.colour_slider2 = gui.HSlider(name="g", value=self.g, 
                                         min=0, max=255, size = 20, width=255, height=16)
        self.colour_label2 = gui.Label("Green: " + str(self.colour_slider2.value))
        self.colour_slider2.connect(gui.CHANGE, self.adjust_colour, 
                                   (self.colour_slider2, self.colour_label2, "Green"))
        tt.add(self.colour_slider2,2,4)
        tt.tr()
        tt.add(self.colour_label2,2,5)
        tt.tr()
        tt.td(gui.Spacer(width=8,height=8))
        self.colour_slider3 = gui.HSlider(name="b", value=self.b, 
                                         min=0, max=255, size = 20, width=255, height=16)
        self.colour_label3 = gui.Label("Blue: " + str(self.colour_slider3.value))
        self.colour_slider3.connect(gui.CHANGE, self.adjust_colour, 
                                   (self.colour_slider3, self.colour_label3, "Blue"))
        tt.add(self.colour_slider3,2,6)
        tt.tr()
        tt.add(self.colour_label3,2,7)
        tt.tr()
        t.td(tt,colspan=2)
        t.tr()
        
        t.td(gui.Spacer(width=8,height=8))
        t.tr()
        self.colour_block = gui.Color(width = 200, height = 20)
        self.colour_block.value = self.setColour
        t.td(self.colour_block, colspan = 2)
        t.tr()
        
        t.td(gui.Spacer(width=8,height=8))

        ##The okay button CLICK event is connected to the Dialog's                       
        ##send event method.  It will send a gui.CHANGE event.                           
        ##::                                                                             
        t.tr()
        e = gui.Button("Okay")
        e.connect(gui.CLICK,self.send,gui.CHANGE)
        t.td(e)
        ##                                                                               

        e = gui.Button("Cancel")
        e.connect(gui.CLICK,self.close,None)
        t.td(e)

        gui.Dialog.__init__(self,title,t)

    def adjust_colour(self, value):
        (slider, label, char) = value
        if char == "Red":
            self.r = slider.value
        if char == "Green":
            self.g = slider.value
        if char == "Blue":
            self.b = slider.value
        self.setColour = pygame.Color(self.r, self.g, self.b, 100)
        self.colour_block.value = self.setColour
        label.set_text(char + ": " + str(slider.value))

class Display():
    def __init__(self, zoom):
        pygame.init()
        
        self.configfile = "config.cfg"
        
        self.config = SafeConfigParser()
        self.config.read([self.configfile])
        
        self.configMinZoom = self.config.getint('zoom', 'min')
        self.configMaxZoom = self.config.getint('zoom', 'max')
        
        self.configTrailR = self.config.getint('trail', 'r')
        self.configTrailG = self.config.getint('trail', 'g')
        self.configTrailB = self.config.getint('trail', 'b')
        
        self.configPointR = self.config.getint('point', 'r')
        self.configPointG = self.config.getint('point', 'g')
        self.configPointB = self.config.getint('point', 'b')
        
        self.configJoystickForward = self.config.getint('joystick', 'forward')
        self.configJoystickTurn = self.config.getint('joystick', 'turn')
        
        self.currentWidth = self.config.getint('window', 'width')
        self.currentHeight = self.config.getint('window', 'height')
        
        if self.currentWidth < 1024:
            self.currentWidth = 1024
        if self.currentHeight < 600:
            self.currentHeight = 600
        
        size = self.can_width, self.can_height = self.currentWidth,self.currentHeight
        
       
        
        self.sgc_screen = sgc.surface.Screen(size)

        self.game_screen = pygame.display.set_mode(size, RESIZABLE)
        global version
        pygame.display.set_caption("Robosim " + version)
        background_colour = (255,255,255)
        self.game_screen.fill(background_colour)

        panel_colour = (200,200,200)
        self.manual_panel = pygame.Surface((1024,100))
        self.manual_panel.set_alpha(200)
        self.manual_panel.fill(panel_colour)

        self.app = gui.App()
        self.app.connect(gui.QUIT,self.app.quit,None)
        self.c = gui.Container(width=self.currentWidth,height=self.currentHeight)
        self.menus = gui.Menus([
            ('File/Load Scenery',self.load_pressed,None),
            ('File/Quit',self.quit_button,None),
#            ('Controls/Keep Centered', self.foo, None),
            ('Controls/log_trail Preferences', self.log_trail_prefs, None),
            ('Controls/set_point Preferences', self.pointer_prefs, None),
            ('Controls/Simulation Preferences', self.sim_prefs, None),
            ('Controls/Reset Robot Position', self.reset_posn, None)
            ])
        ##                                                                               
        self.c.add(self.menus,0,0)
        self.menus.rect.w,self.menus.rect.h = self.menus.resize()
        self.app.init(self.c)


        self.top_panel = pygame.Surface((self.currentWidth,50))
        self.top_panel.set_alpha(200)
        self.top_panel.fill(panel_colour)

        self.clock = pygame.time.Clock()

        self.manual_enabled = False
        self.manual_switch = sgc.Switch(pos=(130,12), label="Controls", label_col=(0,0,0), label_side="bottom",
                                     state=False)
        self.manual_switch.add(order=3)
        self.manual_switch.on_click = self.set_manual 
            
        self.joystick_enabled = False
        self.joystick_switch = sgc.Switch(pos=(130,12), label="Joystick", label_col=(0,0,0), label_side="bottom",
                                     state=False)
        self.joystick_switch.on_click = self.set_joystick                            
        

        t = "Noise Model:\nNone"
        self.noise_label = sgc.Label(pos=(460,7), text=t, col=(0,0,0))
        self.noise_label.add()
        
        self.zoom_scale = sgc.Scale(pos=(270,-10), min=self.configMinZoom, max=self.configMaxZoom, label="Zoom: 10", label_side="bottom", label_col=(0,0,0), show_value=False)
        self.zoom_scale.value=10
        self.zoom_scale.add(order=4)
        self.zoom_scale.on_drag = self.set_zoom

        self.front_ir_switch = sgc.Switch(pos=(400,self.currentHeight - 90), label="Front IR", label_col=(0,0,0), label_side="bottom",
                                     state=False)
        self.front_ir_switch.on_click = self.set_ir_front

        
        self.log_trail_switch = sgc.Switch(pos=(400,self.currentHeight - 90), label="log_trail", label_col=(0,0,0), label_side="bottom",
                                     state=True)
        self.log_trail_switch.on_click = self.set_log_trail

        self.set_point_switch = sgc.Switch(pos=(400,self.currentHeight - 90), label="set_point", label_col=(0,0,0), label_side="bottom",
                                     state=True)
        self.set_point_switch.on_click = self.set_set_point

        self.side_ir_switch = sgc.Switch(pos=(400,self.currentHeight - 60), label="Side IR", label_col=(0,0,0), label_side="bottom",
                                     state=False)
        self.side_ir_switch.on_click = self.set_ir_side

        self.us_switch = sgc.Switch(pos=(400,self.currentHeight - 30), label="Ultrasound", label_col=(0,0,0), label_side="bottom",
                                     state=False)
        self.us_switch.on_click = self.set_us

        self.left_ir_scale = sgc.Scale(pos=(10,self.currentHeight - 60), min=-90, max=90, label="Left IR Angle", 
                                  label_col=(0,0,0), show_value=0, label_side="top")
        self.left_ir_scale.on_drag = self.lservo_set
        self.left_ir_scale.value = 0

        self.right_ir_scale = sgc.Scale(pos=(200, self.currentHeight - 60), min=-90, max=90, label="Right IR Angle", 
                                   label_col=(0,0,0), show_value=0, label_side="top")
        self.right_ir_scale.on_drag = self.rservo_set
        self.right_ir_scale.value = 0

        

        self.joystick = sgc.Joystick(pos=(600,self.currentHeight - 100), min=-10, max=10, label="", label_col=(0,0,0), show_value=False)
        self.joystick.value=0,0
        self.joystick.default_value=0,0
        self.joystick.on_drag = self.jb_set
        
        self.speed_up = 1
        self.speed_up_scale = sgc.Scale(pos=(800, self.currentHeight - 60), min=1, max=100, label="Speed Up", label_col=(0,0,0), show_value=0, label_side="top")
        self.speed_up_scale.on_drag = self.set_speed_up
        
        self.speed_up_scale.value = 1
        
        self.prefs_d = colour_Prefs(self.configTrailR, self.configTrailG, self.configTrailB, "log_trail")

        #fps = sgc.FPSCounter(pos=(300,300), clock=self.clock, label="FPS")
        #fps.add(0)

        self.count = 0
        self.zoom=zoom
        self.x_offset = 0
        self.y_offset = 0
        self.ir_front_used = False
        self.ir_side_used = False
        self.us_used = False
        self.noise_model = SAMPLED_NOISE
        self.noise_value = 0
        self.rand_under = False
        self.display_text = ["UCL","RoboSim"]
        self.create_display()
        self.active = True
        self.timenow = 0
        self.last_active = 0
        self.reposition_UI_elements()
        self.keyx = 0
        self.keyy = 0

    def reposition_UI_elements(self):
        self.front_ir_switch.config(pos = (self.currentWidth/2 - 480,self.currentHeight - 85))
        self.side_ir_switch.config(pos=(self.currentWidth/2 - 480,self.currentHeight - 45))
        self.us_switch.config(pos=(self.currentWidth/2 - 360,self.currentHeight - 85))
        self.joystick_switch.config(pos=(self.currentWidth/2 - 360,self.currentHeight - 45))
        if self.joystick_switch.state:
            self.left_ir_scale.config(pos=(self.currentWidth/2-240,self.currentHeight - 60))
            self.right_ir_scale.config(pos=(self.currentWidth/2 + 80, self.currentHeight - 60))
        else:
            self.left_ir_scale.config(pos=(self.currentWidth/2-175,self.currentHeight - 60))
            self.right_ir_scale.config(pos=(self.currentWidth/2 + 25, self.currentHeight - 60))
        
        self.log_trail_switch.config(pos=(self.currentWidth/2 + 400, self.currentHeight-85))
        self.set_point_switch.config(pos=(self.currentWidth/2 + 400, self.currentHeight - 45))
        
        self.joystick.config(pos=(self.currentWidth/2-50,self.currentHeight - 100))
        self.speed_up_scale.config(pos=(self.currentWidth/2 + 230, self.currentHeight - 60))
        self.top_panel = pygame.transform.scale(self.top_panel,(self.currentWidth,50))
        #self.manual_panel = pygame.transform.scale(self.manual_panel,(self.currentWidth,100))
        
        self.zoom_scale.config(pos=(self.currentWidth/2 - 75,-13))
        self.noise_label.config(pos=(self.currentWidth/2 - 270,7))
        self.manual_switch.config(pos=(self.currentWidth/2 + 110,7))
        #self.joystick_switch.config(pos=(self.currentWidth/2 - 170,7))
        #self.c.menus.remove()
        self.c = gui.Container(width=self.currentWidth,height=self.currentHeight)
        self.c.add(self.menus,0,0)
        self.app.init(self.c)
        
        
    def register_world(self, world):
        self.world = world

    def jb_set(self, x,y):
        norm = sqrt(x * x + y * y)/10.0
        if (norm != 0):
            self.set_active()
            lspeed = (-self.configJoystickForward * y/10.0 + self.configJoystickTurn * x / 10.0)/norm
            rspeed = (-self.configJoystickForward * y/10.0 - self.configJoystickTurn * x / 10.0)/norm
        else:
            lspeed = 0
            rspeed = 0
        self.world.robot.set_speed(L, lspeed)
        self.world.robot.set_speed(R, rspeed)

    def foo(self, dummy):
        pass

    def dialog_event_loop(self):
        self.load_dialog_displayed = True
        while self.load_dialog_displayed:
            self.draw_background()
            self.draw_panels()
            self.process_gui_events()
            self.app.paint()
            pygame.display.flip()

    def load_pressed(self, param):
        self.set_active()
        self.load_d = gui.FileDialog(title_txt="Load Scenery File", button_txt="Load")
        self.load_d.connect(gui.CHANGE, self.handle_file_browser_closed, self.load_d)
        self.load_d.connect(gui.CLOSE, self.leave_dialog_event_loop, None)
        self.load_d.open()
        self.dialog_event_loop()

    def handle_file_browser_closed(self, dlg):
        if dlg.value: input_file = dlg.value
        self.load_d.close()
        parser =  SceneryParser(self, self.world)
        try:
            parser.openfile(input_file)
        except LoadError as e:
            s = "Scenery load failed: " + e.msg
            self.err_d = ErrorDialog(s)
            self.err_d.connect(gui.CLOSE, self.leave_dialog_event_loop, None)
            self.err_d.open()
            self.dialog_event_loop()
                                                                                                  
    def sim_prefs(self, param):
        self.prefs_d = PrefsDialog(self.noise_model, self.noise_value, 
                                   self.rand_under)
        self.prefs_d.connect(gui.CHANGE, self.handle_prefs_dialog_close, self.prefs_d)
        self.prefs_d.connect(gui.CLOSE, self.leave_dialog_event_loop, None)
        self.prefs_d.open()
        self.dialog_event_loop()
        
        
    def log_trail_prefs(self, param):
        self.prefs_d = colour_Prefs(self.configTrailR, self.configTrailG, self.configTrailB, "log_trail")
        self.prefs_d.connect(gui.CHANGE, self.handle_log_trail_dialog_close, self.prefs_d)
        self.prefs_d.connect(gui.CLOSE, self.leave_dialog_event_loop, None)
        self.prefs_d.open()
        self.dialog_event_loop() 
        
    def pointer_prefs(self, param):
        self.prefs_d = colour_Prefs(self.configPointR, self.configPointG, self.configPointB, "set_point")
        self.prefs_d.connect(gui.CHANGE, self.handle_point_dialog_close, self.prefs_d)
        self.prefs_d.connect(gui.CLOSE, self.leave_dialog_event_loop, None)
        self.prefs_d.open()
        self.dialog_event_loop() 
        
    def leave_dialog_event_loop(self, dlg):
        self.load_dialog_displayed = False

    def handle_prefs_dialog_close(self, dlg):
        rand_under = False
        noise_value = 0
        for k,v in dlg.value.items():

            if k=='irnoise':
                noise_model = v
            if k=='noise1' and noise_model == GAUSSIAN_NOISE:
                noise_value = v
            if k=='noise2' and noise_model == ANGULAR_NOISE:
                noise_value = v
            if k=='randunder':
                rand_under = v
        self.set_noise_model(noise_model, noise_value, rand_under)
        self.prefs_d.close()
    
    def handle_log_trail_dialog_close(self, dlg):
        
        for k,v in dlg.value.items():
            temp = str(v)
            if k=='r':
                self.config.set('trail', 'r', temp)
                self.configTrailR = v
            if k=='g':
                self.config.set('trail', 'g', temp)
                self.configTrailG = v
            if k=='b':
                self.config.set('trail', 'b', temp)
                self.configTrailB = v
        with open(self.configfile, "wb") as config_file:
            self.config.write(config_file)
        self.prefs_d.close()
        
    def handle_point_dialog_close(self, dlg):
        
        for k,v in dlg.value.items():
            temp = str(v)
            if k=='r':
                self.config.set('point', 'r', temp)
                self.configPointR = v
            if k=='g':
                self.config.set('point', 'g', temp)
                self.configPointG = v
            if k=='b':
                self.config.set('point', 'b', temp)
                self.configPointB = v
        with open(self.configfile, "wb") as config_file:
            self.config.write(config_file)
        self.prefs_d.close()

    def event_handler(self, foo):
        pass

    def stop_robot(self):
        self.world.robot.stop()

    def fwd_robot(self):
        self.world.robot.fwd(20)

    def back_robot(self):
        self.world.robot.fwd(-20)

    def left_robot(self):
        self.world.robot.turn(-7)

    def right_robot(self):
        self.world.robot.turn(7)

    def create_display(self):
        size = 170,35
        self.lcd_display = pygame.Surface(size)
        self.lcd_display.set_alpha(200)
        darkblue = (0,0,200)
        self.lcd_display.fill(darkblue)
        
        self.font = pygame.font.SysFont("mono", 16)
        white = (255,255,255)
        text1 = self.font.render(self.display_text[0], True, white)
        text2 = self.font.render(self.display_text[1], True, white)
        self.lcd_display.blit(text1, (3,0))
        self.lcd_display.blit(text2, (3,16))

    def set_display_text(self, line, disptext):
        #xxx fix me
        self.display_text[line-1] = disptext[:16]
        self.create_display()

    def set_ir_front(self):
        self.set_active();
        if self.front_ir_switch.state:
            self.world.robot.show_front_ir(True)
        else:
            self.world.robot.show_front_ir(False)

    def set_ir_side(self):
        self.set_active();
        if self.side_ir_switch.state:
            self.world.robot.show_side_ir(True)
        else:
            self.world.robot.show_side_ir(False)

    def set_us(self):
        self.set_active();
        if self.us_switch.state:
            self.world.robot.show_us(True)
        else:
            self.world.robot.show_us(False)

    def set_manual(self):
        self.set_active();
        if self.manual_switch.state:
            self.manual_enabled = True
            self.front_ir_switch.add(order=3)
            self.side_ir_switch.add(order=4)
            self.us_switch.add(order=5)
            self.left_ir_scale.add(order=6)
            self.right_ir_scale.add(order=7)
            
            if self.joystick_switch.state:
                self.joystick.add(order=8)
                self.joystick_enabled = True
            self.speed_up_scale.add(order=9)
            self.joystick_switch.add(order=10)
            self.log_trail_switch.add(order=11)
            self.set_point_switch.add(order=12)
        else:
            self.manual_enabled = False
            self.front_ir_switch.remove()
            self.side_ir_switch.remove()
            self.us_switch.remove()
            self.left_ir_scale.remove()
            self.right_ir_scale.remove()
            self.joystick.remove()
            self.speed_up_scale.remove()
            self.joystick_switch.remove()
            self.log_trail_switch.remove()
            self.set_point_switch.remove()
           
    def set_joystick(self):
        self.set_active();
        if self.joystick_switch.state and self.manual_switch.state:
            self.joystick_enabled = True
            self.joystick.add(order=8)
            self.reposition_UI_elements()
        else:
            self.joystick_enabled = False
            self.reposition_UI_elements()
            self.joystick.remove()
     
    def set_set_point(self):
        self.set_active();

    def set_log_trail(self):
        self.set_active();


    def set_noise_model(self, noise_model, noise_value, rand_under):
        self.set_active();
        self.noise_model = noise_model
        self.noise_value = noise_value
        self.rand_under = rand_under
        if noise_model == NO_NOISE:
            t = "Noise Model:\nNone"
        elif noise_model == SAMPLED_NOISE:
            t = "Noise Model:\nSampled"
        elif noise_model == GAUSSIAN_NOISE:
            t = "Noise Model:\nGaussian (" + str(noise_value) + ")"
        elif noise_model == ANGULAR_NOISE:
            t = "Noise Model:\nAngular (" + str(noise_value) + ")"
        if rand_under:
            t += " + under-reads"
        self.noise_label.config(text=t)
        self.world.robot.set_noise_model(noise_model, noise_value)
        self.world.robot.set_rand_noise(rand_under)

    def set_zoom(self,value):
        self.set_active();
        self.zoom_scale.config(label = "Zoom: " + str(int(round(value))))
        self.zoom=value/10.0
        
    def set_speed_up(self,value):
        self.set_active();
        self.speed_up=value

    def reset_posn(self, dummy):
        self.set_active();
        self.world.robot.reset_posn()
        
    def no_op(self, *args):
        #do nothing
        pass

    def lservo_set(self, value):
        self.set_active();
        self.world.robot.set_ir_angle(L, int(value))

    def rservo_set(self, value):
        self.set_active();
        self.world.robot.set_ir_angle(R, int(value))
        
    def set_sensor_visibility(self, ir_front, ir_side, us, line):
        self.ir_front_used = ir_front
        self.ir_side_used = ir_side
        self.us_used = us

    def set_ir_front_dists(self, left_dist, right_dist):
        #xxx fix me
        return

        if self.ir_front_used or self.ir_front_enable.get() == 1:
            self.left_dist.configure(text=int(left_dist))
            self.right_dist.configure(text=int(right_dist))
        else:
            self.left_dist.configure(text="")
            self.right_dist.configure(text="")

    def set_ir_side_dists(self, left_dist, right_dist):
        #xxx fix me
        return

        if self.ir_side_used or self.ir_side_enable.get() == 1:
            self.side_left_dist.configure(text=int(left_dist))
            self.side_right_dist.configure(text=int(right_dist))
        else:
            self.side_left_dist.configure(text="")
            self.side_right_dist.configure(text="")

    def set_us_dist(self, dist):
        #xxx fix me
        return

        if self.us_used or self.us_enable.get() == 1:
            self.us_dist.configure(text=int(dist))
        else:
            self.us_dist.configure(text="")


    def robot_position(self, x, y):
        self.count = self.count + 1
        return

    def draw_game_grid(self):
        gridcol = (200,200,200)
        xmin = -self.x_offset*self.zoom
        if xmin < 0:
            xmin = 0
        xmax = (2000-self.x_offset)*self.zoom
        if xmax > self.currentWidth:
            xmax = self.currentWidth
        ymin = -self.y_offset*self.zoom
        if ymin < 0:
            ymin = 0
        ymax = (2000-self.y_offset)*self.zoom
#         if self.manual_enabled:
#             if ymax > self.currentHeight-100:
#                 ymax = self.currentHeight-100
#             pygame.draw.line(self.game_screen, gridcol, 
#                              (xmin, ymax), 
#                              (xmax, ymax), 1)
#         else:
        if ymax > self.currentHeight:
            ymax = self.currentHeight
        for i in range(int(100)):
#            if self.manual_enabled:
#                 xval = (i*20 - self.x_offset)*self.zoom
#                 if xval > 0 and xval < self.currentWidth:
#                     pygame.draw.line(self.game_screen, gridcol, 
#                                      (xval, ymin), 
#                                      (xval, ymax), 1)
#                 yval = (i*20 - self.y_offset)*self.zoom
#                 if yval > 0 and yval < self.currentHeight - 100:
#                     pygame.draw.line(self.game_screen, gridcol, 
#                                      (xmin, yval), 
#                                      (xmax, yval), 1)

#             else:
                xval = (i*20 - self.x_offset)*self.zoom
                if xval > 0 and xval < self.currentWidth:
                    pygame.draw.line(self.game_screen, gridcol, 
                                     (xval, ymin), 
                                     (xval, ymax), 1)
                yval = (i*20 - self.y_offset)*self.zoom
                if yval > 0 and yval < self.currentHeight:
                    pygame.draw.line(self.game_screen, gridcol, 
                                     (xmin, yval), 
                                     (xmax, yval), 1)


    def g_screen(self):
        return self.game_screen

    def process_gui_events(self):
        for event in pygame.event.get():
            sgc.event(event)
            if event.type == GUI:
                self.active = True
                pass
            elif event.type == QUIT:
                exit()
            elif event.type == VIDEORESIZE:
                new_size = event.dict['size']
                self.currentWidth = new_size[0]
                self.currentHeight = new_size[1]
                size = self.currentWidth,self.currentHeight
                self.sgc_screen = sgc.surface.Screen(size)
                self.prefs_d.close()
                self.gamescreen=pygame.display.set_mode(event.dict['size'],RESIZABLE)
                self.reposition_UI_elements()
                
                pygame.display.flip()
            elif event.type == pygame.KEYDOWN:
                if event.key == pygame.K_UP or event.key == pygame.K_w:
                    self.keyy = -10
                elif event.key == pygame.K_DOWN or event.key == pygame.K_s:
                    self.keyy = 10
                if event.key == pygame.K_LEFT or event.key == pygame.K_a:
                    if self.keyy > 0:
                        self.keyx = 10
                    else:
                        self.keyx = -10
                elif event.key == pygame.K_RIGHT or event.key == pygame.K_d:
                    if self.keyy > 0:
                        self.keyx = -10
                    else:
                        self.keyx = 10
                if event.key == pygame.K_SPACE:
                    self.reset_posn(self)
            elif event.type == pygame.KEYUP:
                if event.key == pygame.K_UP or event.key == pygame.K_w:
                    self.keyy = 0
                elif event.key == pygame.K_DOWN or event.key == pygame.K_s:
                    self.keyy = 0
                if event.key == pygame.K_LEFT or event.key == pygame.K_a:
                    self.keyx = 0
                elif event.key == pygame.K_RIGHT or event.key == pygame.K_d:
                    self.keyx = 0
                if self.keyx == 0 and self.keyy == 0 and not self.joystick_switch.state:
                    self.jb_set(0,0)
            else:
                self.active = True
                self.app.event(event)
        return self.active

    def draw_background(self):
        background_colour = (255,255,255)
        self.game_screen.fill(background_colour)
        self.draw_game_grid()

    def draw_panels(self):
        self.game_screen.blit(self.top_panel, (0,0))
        self.game_screen.blit(self.lcd_display, (self.currentWidth/2 + 223,7))
        if self.manual_enabled:
            self.game_screen.blit(self.manual_panel, (self.currentWidth/2-512,self.currentHeight -100))

    def update(self, x_offset, y_offset):
        self.x_offset = x_offset
        self.y_offset = y_offset
        self.draw_background()

    def set_active(self):
        self.active = True
        self.last_active = self.timenow

    def flip(self):
        self.draw_panels()
        if self.active:
            #if nothing has happened for 30 seconds, go inactive
            if self.timenow - self.last_active > 5000:
                self.active = False 
        if self.active:
            #print 30
            time = self.clock.tick(self.speed_up*30)
        else:
            #print 5
            time = self.clock.tick(5)
        self.timenow = self.timenow + time
        sgc.update(time)
        self.app.paint()
        pygame.display.flip()

    def quit_button(self, dummy):
        exit()

