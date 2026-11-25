#include <Bluepad32.h>

// =====================================================
// MOTOR DRIVER #1 (DEPAN)
// =====================================================

// RIGHT FRONT MOTOR
#define RF_EN   13
#define RF_IN1  14
#define RF_IN2  27

// LEFT FRONT MOTOR
#define LF_EN   33
#define LF_IN1  25
#define LF_IN2  26

// =====================================================
// MOTOR DRIVER #2 (BELAKANG)
// =====================================================

// LEFT REAR MOTOR
#define LR_EN   32
#define LR_IN1  19
#define LR_IN2  18

// RIGHT REAR MOTOR
#define RR_EN   21
#define RR_IN1  22
#define RR_IN2  23

// =====================================================
// PENGATURAN
// =====================================================
const int PWM_FREQ   = 5000;  // Hz
const int PWM_RES    = 8;     // bit -> duty 0..255
const int DEADZONE   = 40;    // abaikan gerakan stick kecil (axis -511..512)
const bool DEBUG     = false; // true = cetak status ke Serial Monitor

// =====================================================
// STRUKTUR MOTOR
// =====================================================
struct Motor {
    uint8_t en;       // pin PWM (ENA/ENB driver)
    uint8_t in1;
    uint8_t in2;
    bool    invert;   // ubah true jika arah putar motor terbalik
    uint8_t channel;  // channel PWM (dipakai core ESP32 versi 2.x)
};

//               EN      IN1      IN2      invert  ch
Motor motorRF = {RF_EN, RF_IN1, RF_IN2, true,  0};
Motor motorLF = {LF_EN, LF_IN1, LF_IN2, true,  1};
Motor motorRR = {RR_EN, RR_IN1, RR_IN2, false, 3};
Motor motorLR = {LR_EN, LR_IN1, LR_IN2, false, 2};

// =====================================================
// PWM (kompatibel core ESP32 2.x dan 3.x)
// =====================================================
void pwmSetup(Motor &m) {
#if ESP_ARDUINO_VERSION_MAJOR >= 3
    ledcAttach(m.en, PWM_FREQ, PWM_RES);
#else
    ledcSetup(m.channel, PWM_FREQ, PWM_RES);
    ledcAttachPin(m.en, m.channel);
#endif
}

void pwmWrite(Motor &m, int duty) {
#if ESP_ARDUINO_VERSION_MAJOR >= 3
    ledcWrite(m.en, duty);
#else
    ledcWrite(m.channel, duty);
#endif
}

// =====================================================
// KONTROL MOTOR
// speed: -255 (mundur penuh) .. 0 (stop) .. +255 (maju penuh)
// =====================================================
void setMotor(Motor &m, int speed) {
    speed = constrain(speed, -255, 255);
    if (m.invert) speed = -speed;

    if (speed > 0) {
        digitalWrite(m.in1, HIGH);
        digitalWrite(m.in2, LOW);
    } else if (speed < 0) {
        digitalWrite(m.in1, LOW);
        digitalWrite(m.in2, HIGH);
    } else {
        digitalWrite(m.in1, LOW);
        digitalWrite(m.in2, LOW);
    }
    pwmWrite(m, abs(speed));
}

// Sisi kiri = LF + LR, sisi kanan = RF + RR
void driveSides(int leftSpeed, int rightSpeed) {
    setMotor(motorLF, leftSpeed);
    setMotor(motorLR, leftSpeed);
    setMotor(motorRF, rightSpeed);
    setMotor(motorRR, rightSpeed);
}

void stopAll() {
    driveSides(0, 0);
}

// =====================================================
// BLUEPAD32
// =====================================================
ControllerPtr myControllers[BP32_MAX_GAMEPADS];

int R_xvalue = 0;  // stick kanan X (belok)
int L_yvalue = 0;  // stick kiri Y (maju/mundur)

void onConnectedController(ControllerPtr ctl) {
    for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
        if (myControllers[i] == nullptr) {
            Serial.printf("CALLBACK: Controller connected, index=%d\n", i);
            myControllers[i] = ctl;
            break;
        }
    }
}

void onDisconnectedController(ControllerPtr ctl) {
    for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
        if (myControllers[i] == ctl) {
            Serial.printf("CALLBACK: Controller disconnected from index=%d\n", i);
            myControllers[i] = nullptr;
            break;
        }
    }
    // Failsafe: reset input & hentikan mobil
    R_xvalue = 0;
    L_yvalue = 0;
    stopAll();
}

void processGamepad(ControllerPtr ctl) {
    R_xvalue = ctl->axisRX();  // -511 .. 512
    L_yvalue = ctl->axisY();   // -511 .. 512 (atas = negatif)
}

void processControllers() {
    for (auto myController : myControllers) {
        if (myController && myController->isConnected() && myController->hasData()) {
            if (myController->isGamepad()) {
                processGamepad(myController);
            } else {
                Serial.println("Unsupported controller");
            }
        }
    }
}

bool anyControllerConnected() {
    for (auto c : myControllers) {
        if (c && c->isConnected()) return true;
    }
    return false;
}

// Normalisasi axis ke -255..255 dengan deadzone
int normalizeAxis(int v) {
    if (abs(v) < DEADZONE) return 0;
    int sign = (v > 0) ? 1 : -1;
    int mag  = map(abs(v), DEADZONE, 512, 0, 255);
    return sign * constrain(mag, 0, 255);
}

// =====================================================
// SETUP
// =====================================================
void setup() {
    Serial.begin(115200);

    Serial.printf("Firmware: %s\n", BP32.firmwareVersion());
    const uint8_t *addr = BP32.localBdAddress();
    Serial.printf("BD Addr: %02X:%02X:%02X:%02X:%02X:%02X\n",
                  addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]);

    BP32.setup(&onConnectedController, &onDisconnectedController);
    BP32.forgetBluetoothKeys();
    BP32.enableVirtualDevice(false);

    // Inisialisasi semua motor
    Motor *motors[] = {&motorRF, &motorLF, &motorRR, &motorLR};
    for (Motor *m : motors) {
        pinMode(m->in1, OUTPUT);
        pinMode(m->in2, OUTPUT);
        pinMode(m->en, OUTPUT);
        pwmSetup(*m);
    }
    stopAll();
}

// =====================================================
// LOOP
// =====================================================
void loop() {
    bool dataUpdated = BP32.update();
    if (dataUpdated) processControllers();

    // Failsafe: tidak ada controller -> stop
    if (!anyControllerConnected()) {
        stopAll();
        delay(20);
        return;
    }

    // Stick kiri Y: maju (+) / mundur (-). Stick kanan X: belok kanan (+) / kiri (-)
    int throttle = normalizeAxis(-L_yvalue);
    int steer    = normalizeAxis(R_xvalue);

    // Arcade mixing
    int leftSpeed  = constrain(throttle + steer, -255, 255);
    int rightSpeed = constrain(throttle - steer, -255, 255);

    driveSides(leftSpeed, rightSpeed);

    if (DEBUG) {
        Serial.printf("T:%4d S:%4d | L:%4d R:%4d\n", throttle, steer, leftSpeed, rightSpeed);
    }

    delay(20);
}