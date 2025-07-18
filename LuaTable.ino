#include <Arduino.h>
extern "C" {
#include "lua-5.4.8/src/lua.h"
#include "lua-5.4.8/src/lauxlib.h"
#include "lua-5.4.8/src/lualib.h"
}

// Lua interpreter state
static lua_State* L;

// C function exposed to Lua to toggle the on-board LED
static int lua_toggle_led(lua_State* Lstate) {
  static bool state = false;
  state = !state;
  digitalWrite(LED_BUILTIN, state ? HIGH : LOW);
  return 0; // no return values
}

void setup() {
  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);

  // Initialize Lua
  L = luaL_newstate();
  luaL_openlibs(L);
  // Register the LED toggle function
  lua_register(L, "toggle_led", lua_toggle_led);

  // Define a Lua function that will be called each loop
  const char* script = "function loop() toggle_led() end";
  if (luaL_dostring(L, script) != LUA_OK) {
    Serial.print("Lua init error: ");
    Serial.println(lua_tostring(L, -1));
    lua_pop(L, 1);
  }
}

void loop() {
  Serial.println("Arduino main loop running");

  // Call the Lua loop() function
  lua_getglobal(L, "loop");
  if (lua_pcall(L, 0, 0, 0) != LUA_OK) {
    Serial.print("Lua runtime error: ");
    Serial.println(lua_tostring(L, -1));
    lua_pop(L, 1);
  }

  delay(1000); // Wait a bit between iterations
}

