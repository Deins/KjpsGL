/*!
    /file kjpsGL.hpp
    /author Deins Egle
**/

#pragma once
#include <string>
#include <vector>
#include <sstream>
#include <GL/gl.h>
#include <GL/glu.h>

namespace kjpsgl
{
//! 2D Vektors
struct Vec2
{
    float x; //!< x komponente
    float y; //!< y komponente
    Vec2(float x = 0.0f, float y = 0.0f);
    float magnitude(); //!< aprēķina vektora garumu \return vektora garums
    Vec2 normalized(); //!< \return normalizēts vektors (virziena vektors ar garumu 1)
};

//! RGBA krāsa
struct Color
{
    unsigned char r;//!< krāsas sarkanā komponente
    unsigned char g;//!< krāsas zaļā komponente
    unsigned char b;//!< krāsas zilā komponente
    unsigned char a;//!< krāsas alpha (caurspīdības) komponente

    Color(unsigned char r=0, unsigned char g=0, unsigned char b=0, unsigned char a = 255);
};

/*! \brief pārvērš mainīgo par tekstu
    \param v mainīgais kura vērtību pārērst par tekstu
    \return v tekstuālu reprezentāciju
*/
template <typename T>
std::string toString(const T& v)
{
    std::ostringstream ss;
    ss << v;
    return ss.str();
}

/*!
    \brief Inicializē bibliotēku un izveido zīmēšanas logu
    \param width loga platums vai fullscreen režīmā x izšķirtspēja (0 ja vēlas logu atvērt pa visu ekrānu vai fullscreen režīmā izmantot noklusēto monitora izšķirtspēju)
    \param height loga augstums vai fullscreen režīmā y izšķirtspēja (0 ja vēlas logu atvērt pa visu ekrānu vai fullscreen režīmā izmantot noklusēto monitora izšķirtspēju)
    \param fullscreen vai logu atvērt fullscreen režīmā (true=fullscreen;false=windowed)
    \param msaa MSAA anti aliasing kvalitāte. Atļautās vērtības (ja atbalsta video karte) ir 0 (izslēgts), 2, 4, 8, 16 vai 32
    \image html aa.gif "Kā izskatās anti aliasing"
*/
void init(int width = 0, int height = 0, bool fullscreen = false, int msaa = 8);

/*!
    \brief Atsvaidzina bibliotēku
    \details Atsvaidzinot bibliotēku tā saņem visus loga notikumus un citus operētējsistēmas ziņojumus, iegūst svaigu informāciju par tastatūru, peli u.c. ievad ierīcēm.
     Šo funkciju jāizsauc regulāri, citādi operētējsistēma var paziņot ka programma ir "not responding", jo no tās netiek nolasīti ziņojumi.
     Parasti šo funkciju izsauc vienreiz katrā kadrā.
*/
void update();

/*!
    \brief Attēlo uzzīmēto uz ekrāna.
    \defails Bibliotēka veic tehniku sauktu par "Double-Buffering".
    Dobule-Buffering nosaka, ka ir 2 bufferi, 1 no tiem tiek attēlots kamēr otrā notiek zīmēšana.
    display() samaina šos buferus vietām tādējādi attēlojot līdz šim uzzīmēto.
    Double buffering ir vajadzīgs lai novērstu dažādus vizuālus artifaktus un neļautu attēlot pusuzzīmētu attēlu.
    Taču pat ar double buffering iespējama vizuāla nepilnība "Screen tearing", lai to novērstu skatiet setVsync();
    \image html double.png "Double-Buffering vizualizācija"
    \see setVsync()
    \see http://en.wikipedia.org/wiki/Multiple_buffering
*/
void display();

/*!
    \brief nomaina loga virsrakstu
    \param title loga jaunais virsraksts
*/
void setWindowTitle(const std::string& title);
//! \return pašreizējo loga platumu vai fullscreen režīmā izšķirtspēju
int getWindowWidth();
//! \return pašreizējo loga augstumu vai fullscreen režīmā izšķirtspēju
int getWindowHeight();

/*!
    \brief Iegūst laiku kādā tika izpildītas darbības starp pēdējiem diviem update() izsaukumiem (ieskaitot pašu update() izsaukuma laiku)
    \details Šī informācija ir noderīga lai varētu veikt animācijas pareizā ātrumā neatkarīgi no tā cik daudz kadru sekundē dators spējīgs attēlot.
    Piemēram, kustinot bumbu 1px katrā kadrā, bumba tiks pakustināta par 1000px ja dators spēj apstrādāt 1000 kadrus sekundē.
    Taču tikai 100px ja dators ir lēns un spēj apstrādāt tikai 100 kadrus. Šo problēmu var vienkārši atrisināt.
    Ja vēlamies kustināt bumbu 100px sekundē tad 100 kadros sekundē kustinam bumbu katrā kadrā par 1px, ja dators strādā 50 kadros sekundē bumbu katrā kadrā pakustinam pa 2px.
    To vienkārši varam implementēt šādi bumbasPozīcija += ātrums * getDeltaTime(); Tādējādi mēs pielāgojamies datora ātrdarbībai. (tas strādā tikai tad ja update izsauc 1reiz kadrā)
    \return laiks sekundēs cik ilgs pagājis starp pēdējiem diviem update izsaukumiem. (ja update izsauc 1x kadrā, tad tas norāda cik ilgā laikā dators izpildījis pēdējo kadru)
    \see update()
*/
float getDeltaTime();

/*!
    \brief ieslēdz vai izslēdz vsync.
    \param enabled true lai ieslēgtu, false lai izslēgtu
    \details Vsync atrisina vizuālu problēmu sauktu par "Screen tearing".
    Tas notiek tad, ja buferi tiek samainīti laikā kad monitors ir atsvaidzināšanas processā.
    Vsync sinhronizē buferu apmaiņu ar monitora atsvaidzes intensitāti (refresh rate).
    \image html vsync.jpg "Screen tearing piemērs"
    \see display()
    \see http://en.wikipedia.org/wiki/Screen_tearing
*/
void setVsync(bool enabled);

/*! \brief Notīra zīmējamo bufferi.
    \param red notīrāmas krāsas red vērtība [0,255]
    \param green notīrāmas krāsas green vērtība [0,255]
    \param blue notīrāmas krāsas blue vērtība [0,255]*/
void clearScreen(unsigned char red = 0, unsigned char green = 0, unsigned char blue = 0);

/*! \brief Uzstāda zīmēšanas skatu
    \param left skata labā mala (x1 kordinate)
    \param bottom skata apakšējā mala (y1 kordinate)
    \param right skata labā mala (x2 kordinate)
    \param top skata augšējā mala (y2 kordinate)*/
void setView(float left, float bottom, float right, float top);

/*! \brief Uzstāda zīmēšanas krāsu
    \param red krāsas red vērtība [0,255]
    \param green krāsas green vērtība [0,255]
    \param blue krāsas blue vērtība [0,255]
    \param alpha caurspīdība [0(pilnīgi caurspīdīgs), 255(pilnīgi necaurspīdīgs)]*/
void setColor(unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha = 255);

/*! \brief Uzzīmē trijstūri ar virsotnēm A,B,C
    \param ax A x koordinate
    \param ay A y koordinate
    \param bx B x koordinate
    \param by B y koordinate
    \param cx C x koordinate
    \param cy C y koordinate*/
void drawTriangle(float ax, float ay, float bx, float by, float cx, float cy);

//! Uzzīmē taisnstūri
void drawRectangle(float left, float bottom, float right, float top);

/*! \brief Uzzīmē apli
    \param x centra koordinate x
    \param y centra koordinate y
    \param r apļa radius
    \param segments segmentu skaits ko izmantot apļa zīmēšanai*/
void drawCircle(float x, float y, float r, int segments = 64);

/*! \brief Uzzīmē trijstūra kontūru
    \param ax A x koordinate
    \param ay A y koordinate
    \param bx B x koordinate
    \param by B y koordinate
    \param cx C x koordinate
    \param cy C y koordinate
    \param w kontūras platums(resnums)*/
void drawTriangleOutline(float ax, float ay, float bx, float by, float cx, float cy,float w = 1);
//! Uzzīmē taisnstūra kontūru
void drawRectangleOutline(float left, float bottom, float right, float top,float w = 1);
/*! \brief Uzzīmē apļa kontūru
    \param x centra koordinate x
    \param y centra koordinate y
    \param r apļa radius
    \param segments segmentu skaits ko izmantot apļa zīmēšanai
    \param w kontūras platums(resnums)*/
void drawCircleOutline(float x, float y, float r, int segments = 64, float w = 1);

/*! \brief Zīmē līniju
    \param x1 līnijas sākuma pozīcija x
    \param y1 līnijas sākuma pozīcija y
    \param x2 līnijas gala pozīcija x
    \param y2 līnijas gala pozīcija y
    \param w līnijas platums (resnums)
    \param caps līnijas galu noapaļošana (on/off)
    \warning zīmējot caurspīdīgas līnijas ar caps ir redzamas vizuālas nepilnības (līnijas galu noapaļojumi pārklājas ar pašu līniju) */
void drawLine(float x1, float y1, float x2, float y2,float w = 1,bool caps = false);
/*! \brief Zīmē nepārtrauktu lauztu līniju
    \param points līnijas virsotnes
    \param w līnijas platums(resnums)
    \param caps līnijas galu noapaļošana (on/off)
    \warning zīmējot caurspīdīgas līnijas ar caps ir redzamas vizuālas nepilnības (līnijas galu noapaļojumi pārklājas ar pašu līniju) */
void drawLine(const std::vector<Vec2>& points,float w=1,bool caps=true);

/*! \brief Zīmē masīvus (Vertex Arrays)
    \param vertices virsotnes
    \param mode zīmēšanas režīms. Pieejamie režīmi: GL_POINTS,GL_LINE_STRIP,GL_LINE_LOOP,GL_TRIANGLES,GL_TRIANGLE_STRIP,GL_TRIANGLE_FAN,GL_QUADS,GL_QUAD_STRIP,GL_POLYGON;
    \image html glDrawModes.gif "zīmēšanas režīmi" */
void drawArrays(const std::vector<Vec2>& vertices, unsigned mode = GL_TRIANGLES);
/*! Zīmē masīvus (Vertex Arrays)
    \param vertices virsotnes
    \param colors virsotņu krāsas
    \param mode zīmēšanas režīms. Pieejamie režīmi: GL_POINTS,GL_LINE_STRIP,GL_LINE_LOOP,GL_TRIANGLES,GL_TRIANGLE_STRIP,GL_TRIANGLE_FAN,GL_QUADS,GL_QUAD_STRIP,GL_POLYGON;
    \image html glDrawModes.gif "zīmēšanas režīmi" */
void drawArrays(const std::vector<Vec2>& vertices,const std::vector<Color>& colors, unsigned mode = GL_TRIANGLES);
/*! \brief Zīmē masīvus (Vertex Arrays)
    \param vertices virsotnes
    \param uv virsotņu uv (tekstūras) koordinates
    \param mode zīmēšanas režīms. Pieejamie režīmi: GL_POINTS,GL_LINE_STRIP,GL_LINE_LOOP,GL_TRIANGLES,GL_TRIANGLE_STRIP,GL_TRIANGLE_FAN,GL_QUADS,GL_QUAD_STRIP,GL_POLYGON;
    \image html glDrawModes.gif "zīmēšanas režīmi" */
void drawArrays(const std::vector<Vec2>& vertices,const std::vector<Vec2>& uv, unsigned mode = GL_TRIANGLES);
/*! \brief Zīmē masīvus (Vertex Arrays)
    \param vertices virsotnes
    \param uv virsotņu uv (tekstūras) koordinates
    \param colors virsotņu krāsas
    \param mode zīmēšanas režīms. Pieejamie režīmi: GL_POINTS,GL_LINE_STRIP,GL_LINE_LOOP,GL_TRIANGLES,GL_TRIANGLE_STRIP,GL_TRIANGLE_FAN,GL_QUADS,GL_QUAD_STRIP,GL_POLYGON;
    \image html glDrawModes.gif "zīmēšanas režīmi" */
void drawArrays(const std::vector<Vec2>& vertices,const std::vector<Vec2>& uv,const std::vector<Color>& colors, unsigned mode = GL_TRIANGLES);

/*! \brief Ielādē tekstūru
    \details Ielādē attēlu no faila un nosūta to video kartei kā tekstūru.
    \param filename attēla faila nosaukums(ceļš uz failu).
    \return ielādētās tekstūras id.
    \warning return atgrieztais id nav opengl tekstūras id bet gan bibliotēkai specifisks!*/
int loadTexture(const std::string& filename);

/*! \brief Izdzēš iepriekš ielādētu tekstūru
    \param id kuru tekstūru izdzēst*/
void destroyTexture(int id);
/*! \brief Uzstāda zīmēšanai tekstūru.
    \param id kuru tekstūru izmantot zīmēšanai, vai -1 lai neizmantotu nekādu tekstūru zīmēšanai*/
void setTexture(int id = -1);

/*! \brief Uzzina vai kāds tastatūras taustiņš ir nospiests
    \param key taustiņa nosaukums
    \return true ja taustiņš ir nospiests, citādi false
    \details ja taustiņa nosaukums netiek identificēts tiek veikta brīdinājuma izdruka konsolē (std::cerr)
    \details \b Pieejamie \b taustiņu \b nosaukumi:
\li	"0"
\li	"1"
\li	"2"
\li	"3"
\li	"4"
\li	"5"
\li	"6"
\li	"7"
\li	"8"
\li	"9"
\li	"A"
\li	"AC Back" (the Back key (application control keypad))
\li	"AC Bookmarks" (the Bookmarks key (application control keypad))
\li	"AC Forward" (the Forward key (application control keypad))
\li	"AC Home" (the Home key (application control keypad))
\li	"AC Refresh" (the Refresh key (application control keypad))
\li	"AC Search" (the Search key (application control keypad))
\li	"AC Stop" (the Stop key (application control keypad))
\li	"Again" (the Again key (Redo))
\li	"AltErase" (Erase-Eaze)
\li	"'"
\li	"Application" (the Application / Compose / Context Menu (Windows) key )
\li	"AudioMute" (the Mute volume key)
\li	"AudioNext" (the Next Track media key)
\li	"AudioPlay" (the Play media key)
\li	"AudioPrev" (the Previous Track media key)
\li	"AudioStop" (the Stop media key)
\li	"B"
\li	"\" (Located at the lower left of the return key on ISO keyboards and at the right end of the QWERTY row on ANSI keyboards. Produces REVERSE SOLIDUS (backslash) and VERTICAL LINE in a US layout, REVERSE SOLIDUS and VERTICAL LINE in a UK Mac layout, NUMBER SIGN and TILDE in a UK Windows layout, DOLLAR SIGN and POUND SIGN in a Swiss German layout, NUMBER SIGN and APOSTROPHE in a German layout, GRAVE ACCENT and POUND SIGN in a French Mac layout, and ASTERISK and MICRO SIGN in a French Windows layout.)
\li	"Backspace"
\li	"BrightnessDown" (the Brightness Down key)
\li	"BrightnessUp" (the Brightness Up key)
\li	"C"
\li	"Calculator" (the Calculator key)
\li	"Cancel"
\li	"CapsLock"
\li	"Clear"
\li	"Clear / Again"
\li	","
\li	"Computer" (the My Computer key)
\li	"Copy"
\li	"CrSel"
\li	"CurrencySubUnit" (the Currency Subunit key)
\li	"CurrencyUnit" (the Currency Unit key)
\li	"Cut"
\li	"D"
\li	"DecimalSeparator" (the Decimal Separator key)
\li	"Delete"
\li	"DisplaySwitch" (display mirroring/dual display switch, video mode switch)
\li	"Down" (the Down arrow key (navigation keypad))
\li	"E"
\li	"Eject" (the Eject key)
\li	"End"
\li	"="
\li	"Escape" (the Esc key)
\li	"Execute"
\li	"ExSel"
\li	"F"
\li	"F1"
\li	"F10"
\li	"F11"
\li	"F12"
\li	"F13"
\li	"F14"
\li	"F15"
\li	"F16"
\li	"F17"
\li	"F18"
\li	"F19"
\li	"F2"
\li	"F20"
\li	"F21"
\li	"F22"
\li	"F23"
\li	"F24"
\li	"F3"
\li	"F4"
\li	"F5"
\li	"F6"
\li	"F7"
\li	"F8"
\li	"F9"
\li	"Find"
\li	"G"
\li	"`" (Located in the top left corner (on both ANSI and ISO keyboards). Produces GRAVE ACCENT and TILDE in a US Windows layout and in US and UK Mac layouts on ANSI keyboards, GRAVE ACCENT and NOT SIGN in a UK Windows layout, SECTION SIGN and PLUS-MINUS SIGN in US and UK Mac layouts on ISO keyboards, SECTION SIGN and DEGREE SIGN in a Swiss German layout (Mac: only on ISO keyboards), CIRCUMFLEX ACCENT and DEGREE SIGN in a German layout (Mac: only on ISO keyboards), SUPERSCRIPT TWO and TILDE in a French Windows layout, COMMERCIAL AT and NUMBER SIGN in a French Mac layout on ISO keyboards, and LESS-THAN SIGN and GREATER-THAN SIGN in a Swiss German, German, or French Mac layout on ANSI keyboards.)
\li	"H"
\li	"Help"
\li	"Home"
\li	"I"
\li	"Insert" (insert on PC, help on some Mac keyboards (but does send code 73, not 117))
\li	"J"
\li	"K"
\li	"KBDIllumDown" (the Keyboard Illumination Down key)
\li	"KBDIllumToggle" (the Keyboard Illumination Toggle key)
\li	"KBDIllumUp" (the Keyboard Illumination Up key)
\li	"Keypad 0" (the 0 key (numeric keypad))
\li	"Keypad 00" (the 00 key (numeric keypad))
\li	"Keypad 000" (the 000 key (numeric keypad))
\li	"Keypad 1" (the 1 key (numeric keypad))
\li	"Keypad 2" (the 2 key (numeric keypad))
\li	"Keypad 3" (the 3 key (numeric keypad))
\li	"Keypad 4" (the 4 key (numeric keypad))
\li	"Keypad 5" (the 5 key (numeric keypad))
\li	"Keypad 6" (the 6 key (numeric keypad))
\li	"Keypad 7" (the 7 key (numeric keypad))
\li	"Keypad 8" (the 8 key (numeric keypad))
\li	"Keypad 9" (the 9 key (numeric keypad))
\li	"Keypad A" (the A key (numeric keypad))
\li	"Keypad &" (the & key (numeric keypad))
\li	"Keypad @" (the @ key (numeric keypad))
\li	"Keypad B" (the B key (numeric keypad))
\li	"Keypad Backspace" (the Backspace key (numeric keypad))
\li	"Keypad Binary" (the Binary key (numeric keypad))
\li	"Keypad C" (the C key (numeric keypad))
\li	"Keypad Clear" (the Clear key (numeric keypad))
\li	"Keypad ClearEntry" (the Clear Entry key (numeric keypad))
\li	"Keypad :" (the : key (numeric keypad))
\li	"Keypad ," (the Comma key (numeric keypad))
\li	"Keypad D" (the D key (numeric keypad))
\li	"Keypad &&" (the && key (numeric keypad))
\li	"Keypad ||" (the || key (numeric keypad))
\li	"Keypad Decimal" (the Decimal key (numeric keypad))
\li	"Keypad /" (the / key (numeric keypad))
\li	"Keypad E" (the E key (numeric keypad))
\li	"Keypad Enter" (the Enter key (numeric keypad))
\li	"Keypad =" (the = key (numeric keypad))
\li	"Keypad = (AS400)" (the Equals AS400 key (numeric keypad))
\li	"Keypad !" (the ! key (numeric keypad))
\li	"Keypad F" (the F key (numeric keypad))
\li	"Keypad >" (the Greater key (numeric keypad))
\li	"Keypad #" (the # key (numeric keypad))
\li	"Keypad Hexadecimal" (the Hexadecimal key (numeric keypad))
\li	"Keypad {" (the Left Brace key (numeric keypad))
\li	"Keypad (" (the Left Parenthesis key (numeric keypad))
\li	"Keypad <" (the Less key (numeric keypad))
\li	"Keypad MemAdd" (the Mem Add key (numeric keypad))
\li	"Keypad MemClear" (the Mem Clear key (numeric keypad))
\li	"Keypad MemDivide" (the Mem Divide key (numeric keypad))
\li	"Keypad MemMultiply" (the Mem Multiply key (numeric keypad))
\li	"Keypad MemRecall" (the Mem Recall key (numeric keypad))
\li	"Keypad MemStore" (the Mem Store key (numeric keypad))
\li	"Keypad MemSubtract" (the Mem Subtract key (numeric keypad))
\li	"Keypad -" (the - key (numeric keypad))
\li	"Keypad *" (the * key (numeric keypad))
\li	"Keypad Octal" (the Octal key (numeric keypad))
\li	"Keypad %" (the Percent key (numeric keypad))
\li	"Keypad ." (the . key (numeric keypad))
\li	"Keypad +" (the + key (numeric keypad))
\li	"Keypad +/-" (the +/- key (numeric keypad))
\li	"Keypad ^" (the Power key (numeric keypad))
\li	"Keypad }" (the Right Brace key (numeric keypad))
\li	"Keypad )" (the Right Parenthesis key (numeric keypad))
\li	"Keypad Space" (the Space key (numeric keypad))
\li	"Keypad Tab" (the Tab key (numeric keypad))
\li	"Keypad |" (the | key (numeric keypad))
\li	"Keypad XOR" (the XOR key (numeric keypad))
\li	"L"
\li	"Left Alt" (alt, option)
\li	"Left Ctrl"
\li	"Left" (the Left arrow key (navigation keypad))
\li	"["
\li	"Left GUI" (windows, command (apple), meta)
\li	"Left Shift"
\li	"M"
\li	"Mail" (the Mail/eMail key)
\li	"MediaSelect" (the Media Select key)
\li	"Menu"
\li	"-"
\li	"ModeSwitch" (I'm not sure if this is really not covered by any of the above, but since there's a special KMOD_MODE for it I'm adding it here)
\li	"Mute"
\li	"N"
\li	"Numlock" (the Num Lock key (PC) / the Clear key (Mac))
\li	"O"
\li	"Oper"
\li	"Out"
\li	"P"
\li	"PageDown"
\li	"PageUp"
\li	"Paste"
\li	"Pause" (the Pause / Break key)
\li	"."
\li	"Power" (The USB document says this is a status flag, not a physical key - but some Mac keyboards do have a power key.)
\li	"PrintScreen"
\li	"Prior"
\li	"Q"
\li	"R"
\li	"Right Alt" (alt gr, option)
\li	"Right Ctrl"
\li	"Return" (the Enter key (main keyboard))
\li	"Return"
\li	"Right GUI" (windows, command (apple), meta)
\li	"Right" (the Right arrow key (navigation keypad))
\li	"]"
\li	"Right Shift"
\li	"S"
\li	"ScrollLock"
\li	"Select"
\li	";"
\li	"Separator"
\li	"/"
\li	"Sleep" (the Sleep key)
\li	"Space" (the Space Bar key(s))
\li	"Stop"
\li	"SysReq" (the SysReq key)
\li	"T"
\li	"Tab" (the Tab key)
\li	"ThousandsSeparator" (the Thousands Separator key)
\li	"U"
\li	"Undo"
\li	"" (no name, empty string)
\li	"Up" (the Up arrow key (navigation keypad))
\li	"V"
\li	"VolumeDown"
\li	"VolumeUp"
\li	"W"
\li	"WWW" (the WWW/World Wide Web key)
\li	"X"
\li	"Y"
\li	"Z"
\li	"&"
\li	"*"
\li	"@"
\li	"^"
\li	":"
\li	"$"
\li	"!"
\li	">"
\li	"#"
\li	"("
\li	"<"
\li	"%"
\li	"+"
\li	"?"
\li	"""
\li	")"
*/
bool getKey(const std::string& key);

/*! \brief Uzzina vai peles poga ir nospiesta
    \param button peles pogas identifikators (1 - kreisā, 2 - vidējā, 3 - labā)
    \return true ja konkrētā peles poga ir nospiesta, citādi false */
bool getMouseButton(int button = 1);
/*! \brief Iegūst peles x pozīciju relatīvi loga apakšējajam kreisajam stūrim.
    \return peles x pozīciju */
float getMouseX();
/*! \brief Iegūst peles y pozīciju relatīvi loga apakšējajam kreisajam stūrim.
    \return peles y pozīciju */
float getMouseY();
/*! \brief Iegūst peles riteņa pagriezienu daudzumu kopš iepriekšējā update() izsaukšanas
    \return peles riteņa pagriešanās daudzumu  */
int getMouseWheel();
}
