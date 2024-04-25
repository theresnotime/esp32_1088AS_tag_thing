const int version_major = 2;
const int version_minor = 3;
const String version_str = String(version_major) + "." + String(version_minor);
const int long_hold = 8000;
const int med_hold = 5000;
const int wipe_speed = 40;
const int scroll_speed = 50;
const int screen_intensity = 1;
const String api_url = "https://fedihashbadge.staged-on.workers.dev/wikis.world/wmhack";
const String github_url = "https://github.com/theresnotime/esp32_1088AS_tag_thing";


// Not a constant, but a global variable
bool wifi_enabled = false;