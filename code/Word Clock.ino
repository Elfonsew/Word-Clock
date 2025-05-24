/*********************************************************************
  ESP32 Word‑Clock  ▸ 130 × WS2812 on GPIO 23  ▸  Pacific Time (PST/PDT)
  • Syncs NTP at boot and every day at 04:00 (local).
  • Wi‑Fi radio is on only while syncing.
  • Every 5 s it prints the local date‑time and the phrase the LEDs show.
*********************************************************************/

#include <FastLED.h>
#include <WiFi.h>
#include <time.h>

// ───────────────────────── USER‑CONFIG ────────────────────────────
#define LED_PIN       23
#define NUM_LEDS      130
#define LED_TYPE      WS2812
#define COLOR_ORDER   GRB
#define BRIGHTNESS    255                       // runtime brightness
constexpr CRGB CLOCK_COLOR = CRGB(255, 229, 5); // warm yellow (no blue)

const char* SSID     = "nvjumpstarter-iot";
const char* PASSWORD = "[J28gYtl!VYw";
const char* NTP_POOL = "pool.ntp.org";

// ───────────────────────── GLOBALS ────────────────────────────────
CRGB leds[NUM_LEDS];
int  lastSyncDay = -1;
bool displayOn   = true;

// ───────────────────────── LED‑WORD MAPS ──────────────────────────
struct LEDSegment { uint8_t start, end; const char* word; };

const LEDSegment commonWords[] PROGMEM = {
  {  2,   4, "IT"      }, {  1,   2, "IS"      },
  {  5,   5, "A"       }, {122, 130, "O'CLOCK" }
};

const LEDSegment minuteWords[] PROGMEM = {
  {16,21,"FIVE"}, {37,40,"TEN"},  { 6,15,"QUARTER"},
  {22,30,"TWENTY"}, {31,36,"HALF"},
  {41,42,"TO"},    {49,54,"PAST"}
};

const LEDSegment hourWords[] PROGMEM = {
  {55, 58,"ONE"},   {70, 73,"TWO"},   {63, 69,"THREE"},
  {80, 85,"FOUR"},  {74, 79,"FIVE"},  {59, 62,"SIX"},
  {111,117,"SEVEN"},{86, 92,"EIGHT"}, {43, 48,"NINE"},
  {118,121,"TEN"},  {93,101,"ELEVEN"},{102,110,"TWELVE"}
};

// ───────────────────────── Wi‑Fi  &  TIME ─────────────────────────
void connectWiFi() {
  Serial.print(F("Connecting to Wi‑Fi"));
  WiFi.begin(SSID, PASSWORD);
  while (WiFi.status() != WL_CONNECTED) { Serial.print('.'); delay(350); }
  Serial.println(F(" connected."));
}

void syncTime() {
  configTzTime("PST8PDT,M3.2.0/2,M11.1.0/2", NTP_POOL);
  struct tm t;
  while (!getLocalTime(&t)) { Serial.println(F("Waiting for NTP…")); delay(500); }
  Serial.printf("✅ NTP sync: %04d‑%02d‑%02d  %02d:%02d:%02d  (%s)\n",
                t.tm_year+1900, t.tm_mon+1, t.tm_mday,
                t.tm_hour, t.tm_min, t.tm_sec,
                t.tm_isdst ? "PDT" : "PST");
}

// ───────────────────────── WORD HELPERS ──────────────────────────
String getHourWord(int h) {
  static const char* H[] = {"ONE","TWO","THREE","FOUR","FIVE","SIX",
                            "SEVEN","EIGHT","NINE","TEN","ELEVEN","TWELVE"};
  return (h>=1 && h<=12) ? String(H[h-1]) : "";
}
String getMinuteWord(int m) {
  switch (m) {
    case  5: return "FIVE";        case 10: return "TEN";
    case 15: return "QUARTER";     case 20: return "TWENTY";
    case 25: return "TWENTY‑FIVE"; case 30: return "HALF";
    default: return "";
  }
}

// ───────────────────────── LED HELPERS ───────────────────────────
void lightSegment(const LEDSegment& s, const CRGB& c) {
  for (uint8_t i = s.start-1; i < s.end; ++i) leds[i] = c;
}
void lightMinute(int m, const CRGB& c) {
  switch (m) {
    case  5: lightSegment(minuteWords[0], c); break;
    case 10: lightSegment(minuteWords[1], c); break;
    case 15: lightSegment(minuteWords[2], c); break;
    case 20: lightSegment(minuteWords[3], c); break;
    case 25: lightSegment(minuteWords[3], c); lightSegment(minuteWords[0], c); break;
    case 30: lightSegment(minuteWords[4], c); break;
  }
}
void lightHour(int h, const CRGB& c) { if (h>=1 && h<=12) lightSegment(hourWords[h-1], c); }

// ───────────────────────── DRAW CLOCK ────────────────────────────
void updateClock(int hour12, int roundedMin) {
  FastLED.clear();
  CRGB col = CLOCK_COLOR;

  lightSegment(commonWords[0], col);   // IT
  lightSegment(commonWords[1], col);   // IS

  if (roundedMin == 0) {                     // … O'CLOCK
    lightHour(hour12, col);
    lightSegment(commonWords[3], col);
  } else if (roundedMin <= 30) {             // … PAST …
    lightMinute(roundedMin, col);
    lightSegment(minuteWords[6], col);       // PAST
    lightHour(hour12, col);
  } else {                                   // … TO …
    int toMin = 60 - roundedMin;
    lightMinute(toMin, col);
    lightSegment(minuteWords[5], col);       // TO
    lightHour((hour12 % 12) + 1, col);
  }
  FastLED.show();
}

// ───────────────────────── SETUP ─────────────────────────────────
void setup() {
  Serial.begin(115200); delay(600);

  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS)
         .setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(BRIGHTNESS);
  FastLED.clear(); FastLED.show();

  /*  power‑on red wipe at brightness 128  */
  uint8_t saveBright = FastLED.getBrightness();
  FastLED.setBrightness(128);
  for (int i=0;i<NUM_LEDS;++i){ leds[i]=CRGB::Red; FastLED.show(); delay(22); }
  FastLED.clear(); FastLED.show();
  FastLED.setBrightness(saveBright);

  connectWiFi();
  syncTime();
  WiFi.disconnect(true);
}

// ───────────────────────── LOOP ──────────────────────────────────
void loop() {
  struct tm now;
  if (!getLocalTime(&now)) { Serial.println(F("!! time read failed")); delay(5000); return; }

  int hour24   = now.tm_hour;
  int minute   = now.tm_min;
  int today    = now.tm_mday;

  /* ── round to nearest 5 min ─────────────────────────── */
  int roundedMin = ((minute + 2) / 5) * 5;   // +2 → nearest
  int hour12;

  if (roundedMin == 60) {            // 58,59 round up → 0 next hour
    roundedMin = 0;
    ++hour24;                        // advance for resync check
  }
  if (hour24 == 24) hour24 = 0;      // midnight rollover

  hour12 = (hour24 == 0) ? 12 : (hour24 > 12 ? hour24 - 12 : hour24);

  /* ── daily 04:00 resync ─────────────────────────────── */
  if (hour24==4 && minute==0 && today!=lastSyncDay) {
    Serial.println(F("\n🔄 04:00 resync"));
    FastLED.clear(); FastLED.show();
    connectWiFi();   syncTime();   WiFi.disconnect(true);
    lastSyncDay = today;
    displayOn   = false;
    delay(61000);                       // coast past :00
  }
  if (!displayOn && (hour24!=4 || minute!=0)) displayOn = true;

  /* ── update LEDs & serial ───────────────────────────── */
  if (displayOn) {
    String phrase;
    if (roundedMin==0)
      phrase = "IT IS " + getHourWord(hour12) + " O'CLOCK";
    else if (roundedMin<=30)
      phrase = "IT IS " + getMinuteWord(roundedMin) + " PAST " + getHourWord(hour12);
    else
      phrase = "IT IS " + getMinuteWord(60-roundedMin) + " TO " + getHourWord((hour12%12)+1);

    Serial.printf("⏰ %04d‑%02d‑%02d  %02d:%02d:%02d  (%s)\n",
                  now.tm_year+1900, now.tm_mon+1, now.tm_mday,
                  hour24, minute, now.tm_sec,
                  now.tm_isdst ? "PDT" : "PST");
    Serial.println("🧠 Clock says: " + phrase);

    updateClock(hour12, roundedMin);
  }
  delay(1000);
}