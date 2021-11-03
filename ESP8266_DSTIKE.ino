/* 
 *  Fecha:      01.11.21
 * Autor:       Raymundo Barrales
 * Título:      Programa de pruebas para la pulsera DSTIKE ESP8266
 * Descripción: Este programa genera un menú de pruebas par probar el LED blanco, el LED RGB y buscar una red para que el DSTIKE se conecte como cliente
 * 
 */

 // Bibliotecas
 //...para el WiFi ESP8266
 #include <ESP8266WiFi.h>
 // FastLED de Daniel García (ver. 3.4.0) para el LED RGB WS2812B
 #include <FastLED.h>
 //Anti-rebotes de Thomas O'Fredericks et al. (ver. 2.57.0) para el botón de selección
 #include <Bounce2.h>
 // Visualización para la pantalla OLED SH1106 de Alexey Dyna (ver. 1.0.6)
 #include <lcdgfx.h>
 #include <lcdgfx_gui.h>

 // Constantes globales
 // Definición de los pines del botón seleccionador según https://www.banggood.com/custlink/vvmY8U1FT6
 const byte displaySDAPin = 5;
 const byte displaySCLPin = 4;
 const byte selBotonArribaPin = 12;
 const byte selBotonAbajoPin = 13;
 const byte selBotonClicPin = 14;
 const byte RGBledPin = 15;
 const byte LEDblancoPin = 16;
 // Opciones del menú de prueba
 const char *menuOpciones[] = {"Prueba LED", "Prueba LED RGB", "Prueba Wi-Fi"};

 // Variables globales
 // Se define un arreglo de un solo elemento que contenga el valor RGB del LED frontal
 CRGB RGBled[1];
 // El DSTIKE utiliza un bus I2C para controlar la pantalla OLED SH1106 sobre la dirección 0x3C
 DisplaySH1106_128x64_I2C display(-1, {-1, 0x3C, displaySCLPin, displaySDAPin, -1});
 // Creación del menú
 LcdGfxMenu menu(menuOpciones, sizeof(menuOpciones)/sizeof(char *));

// Objetos
// Creación de los objetos para el anti-rebotes del botón selector
Bounce2::Button selBotonArriba = Bounce2::Button();
Bounce2::Button selBotonAbajo = Bounce2::Button();
Bounce2::Button selBotonClic = Bounce2::Button();
 
void setup() {
  // Configura el LED blanco
  pinMode(LEDblancoPin, HIGH);

  // Configura el LED RGB
  FastLED.addLeds<WS2812B, 15, GRB > (RGBled, 1);
  RGBled[0] = CRGB::Black;
  FastLED.show();

  // Configura la pantalla OLED
  display.begin();
  display.setFixedFont (ssd1306xled_font6x8);
  display.fill(0x00);
  display.clear();

  // Vincula los objetos anti-rebote a los botones
  selBotonArriba.attach(selBotonArribaPin, INPUT_PULLUP);
  selBotonAbajo.attach(selBotonAbajoPin, INPUT_PULLUP);
  selBotonClic.attach(selBotonClicPin, INPUT_PULLUP);
} // Fin del setup

void loop() {
  // Verifica si cualquier botón ha sido pulsado
  selBotonArriba.update();
  selBotonAbajo.update();
  selBotonClic.update();

  // Selecciones del menú
  if (selBotonAbajo.pressed()) {menu.down();}
  else if (selBotonArriba.pressed()) {menu.up();}
  // Atender la opción seleccionada
  else if (selBotonClic.pressed()) {
    switch (menu.selection()) {
      
      case 0:
      // Encender el LED blanco (LOW = ON)
      digitalWrite(LEDblancoPin, LOW);
      // Pausa
      delay(1000);
      // Apagar de nuevo
      digitalWrite(LEDblancoPin, HIGH);
      break;

      case 1:
      // El LED RGB pasa por todos los colores
      for(int tono = 0; tono < 255; tono++){
        RGBled[0] = CHSV(tono, 255, 255);
        FastLED.show();
        delay(10);
      } // Fin del case 0 y case 1
      // Apagar el LED RGB de nuevo
      RGBled[0] = CRGB(0,0,0);
      FastLED.show();
      break;

      case 2: {
        display.clear();
        display.printFixed(0,0,"Buscando redes..", STYLE_NORMAL);
        // Configurar como un cliente Wi-Fi (modo"stación")
        WiFi.mode(WIFI_STA);
        // Se desconecta para borra valores ocultos de alguna conexión previa
        WiFi.disconnect();
        // Busca info de cualquier red disponible
        int numRedes = WiFi.scanNetworks();
        // Convierte entero a caracter para visualización
        char cstr[2];
        itoa(numRedes, cstr, 10);
        // Visualizar el resultado
        display.clear();
        display.printFixed(0,0,cstr, STYLE_NORMAL);
        display.printFixed(12,0, "Redes encontradas:", STYLE_NORMAL);
        for (int i = 0; i < numRedes; i++) {
          display.printFixed(0, (i+1)*8, WiFi.SSID(i).c_str(), STYLE_NORMAL);
        } // Fin del for
        delay(3000);
        display.clear();
        break;
      } // Fin del case 2

       default:
       break;
    } // Fin del switch
  } // Termina de atender botón clic
  menu.show(display);
} // Fin del loop
