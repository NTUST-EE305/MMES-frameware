
#include "mbed.h"
#include "main.h"

#include "FunctionPointer.h"
#include "rtos.h"

#include "Selection.h"
#include "Menu.h"
#include "Navigator.h"
#include <string>

#include "state.h"
#include "analysis.h"

#include "ConfigFile.h"
ConfigFile cfg;

#include "SDFileSystem.h"
SDFileSystem sd(p5, p6, p7, P2_2, "sd"); // the pinout on the mbed Cool Components workshop board

DigitalOut led2(P0_22);
void led2_thread(void const *args) {
    while (true) {
        led2 = !led2;
        Thread::wait(500);
    }
}

void ain_thread(void const *args) {
    while (true) {
//        pc.printf("normalized: 0x%04X \r\n", ain.read_u16() >> 4);
//        Thread::wait(500);
    }
}

void test_sdcf() {

    pc.printf("Hello World!\n");

    mkdir("/sd/mydir", 0777);

    FILE *fp = fopen("/sd/mydir/sdtest.txt", "w");
    if (fp == NULL) {
        pc.printf("Could not open file for write\n");
    }
    fprintf(fp, "Hello fun SD Card World!");
    fclose(fp);

    pc.printf("Goodbye World!\n");

    char *key = "MyKey";
    char value[BUFSIZ];
    /*
     * Read a configuration file from a mbed.
     */
    if (!cfg.read("/sd/output.cfg")) {
        pc.printf("Failure to read a configuration file.\n");
    }

    /*
     * Get a configuration value.
     */
    if (cfg.getValue(key, &value[0], sizeof(value))) {
        pc.printf("'%s'='%s'\n", key, value);
    }

    /*
     * Set a configuration value.
     */
    if (!cfg.setValue("MyKey", "TestValue")) {
        pc.printf("Failure to set a value.\n");
    }

    /*
     * Write to a file.
     */
    if (!cfg.write("/sd/output.cfg")) {
        pc.printf("Failure to write a configuration file.\n");
    }
}

I2C i2c(p28, p27);
const int addr = 0xA0;

void test_rom() {
    pc.printf("rom\r\n");


    char data[10];
    data[0] = 0x00;
    data[1] = 0x00;
    data[2] = 0x12;
    data[3] = 0x34;
    data[4] = 0x56;
    data[5] = 0x78;
    data[6] = 0x90;
    data[7] = 0xAA;
    data[8] = 0xBB;
    data[9] = 0xCC;
    data[10] = 0xDD;
    i2c.write(addr, data, 10);

    char cmd[2];
    cmd[0] = 0x00;
    cmd[1] = 0x00;
    i2c.write(addr, cmd, 2);
    uint8_t i = 0;
    for (i = 0; i < 10; i++) {
        i2c.read(addr | 0x01, cmd, 1);
        pc.printf("%x\r\n", cmd[0]);
    }
}

int main() {
    pc.baud(115200);

    test_rom();

    pc.printf("Booting...\r\n");

    State state(&lcd, &joystick);

    lcd.setPower(true);
    lcd.cls();
    lcd.setAddress(0, 0);
    lcd.printf("Taiwan Tech  UNI");
    lcd.setAddress(0, 1);
    lcd.printf(" Motor Detector");
    wait(1);

    Thread thread_led(led2_thread);
//    Thread thread_ain(ain_thread);

//User Menu
    Menu menu_root(" Motor Detector", NULL);

    FunctionPointer fun_iso(&test_ISO);
    FunctionPointer fun_nema(&test_NEMA);

    //Menu - Run Test
    Menu menu_test(" Run Test", &menu_root);
    menu_test.add(Selection(&fun_iso, 0, NULL, " ISO-10816")); // ISO-10816
    menu_test.add(Selection(&fun_nema, 1, NULL, " NEMA MG1")); // NEMA MG1

    //Menu - Status
    Menu menu_status(" Status", &menu_root);
    menu_status.add(Selection(NULL, 0, NULL, " OK"));
    menu_status.add(Selection(NULL, 1, NULL, " XYZ"));

    FunctionPointer fun_sd(&state, &State::setting_date);
    //Menu - Setting
    Menu menu_setting(" Setting", &menu_root);
    menu_setting.add(Selection(NULL, 0, NULL, " inch/mm"));
    menu_setting.add(Selection(NULL, 1, NULL, " USB"));
    menu_setting.add(Selection(NULL, 2, NULL, " Network"));
    menu_setting.add(Selection(&fun_sd, 3, NULL, "Date/Time"));

    //Menu - About
    Menu menu_about(" About", &menu_root);
    menu_about.add(Selection(NULL, 0, NULL, " NTUST EE-305"));
    menu_about.add(Selection(NULL, 1, NULL, " Version: 0.1"));

    // Selections to the root menu should be added last
    menu_root.add(Selection(NULL, 0, &menu_test, menu_test.menuID));
    menu_root.add(Selection(NULL, 1, &menu_status, menu_status.menuID));
    menu_root.add(Selection(NULL, 2, &menu_setting, menu_setting.menuID));
    menu_root.add(Selection(NULL, 3, &menu_about, menu_about.menuID));

    // Here is the heart of the system: the navigator.
    // The navigator takes in a reference to the root, an interface, and a reference to an lcd
    Navigator navigator(&menu_root, &lcd);

    FunctionPointer fun_none(&navigator, &Navigator::actionNone);
    FunctionPointer fun_up(&navigator, &Navigator::actionUp);
    FunctionPointer fun_down(&navigator, &Navigator::actionDown);
    FunctionPointer fun_back(&navigator, &Navigator::actionBack);
    FunctionPointer fun_enter(&navigator, &Navigator::actionEnter);
    FunctionPointer fun_home(&navigator, &Navigator::actionHome);

    joystick.functions(&fun_none, &fun_up, &fun_down, &fun_back, &fun_enter, &fun_home);

    while (true) {
        joystick.poll();
    }
}
