#include <FastLED.h>
// how fast to move the dots, in frames per second, adjust this to 
// make the dots move faster or slower
const unsigned short c_Speed = 100U;
// Change this to adjust how long of a tail a dot will have
const unsigned short c_FadeTail = 20U;
const unsigned short c_NumberOfLEDs = 100U;
const unsigned short c_Stripes = 4U;
const unsigned short c_Saturation = 63U;
const unsigned short c_Value = 83U;
const unsigned short c_ButtonOffset = 10U;
//debug flag
const boolean c_WriteToSerial = false;
//button status
boolean knopfGedryckt[ c_Stripes ];
//2d array for colors
CRGB leds[ c_Stripes ][ c_NumberOfLEDs ];

void setup()
{
  FastLED.addLeds< APA102, 1, 2 >( leds[ 0 ], c_NumberOfLEDs );
  FastLED.addLeds< APA102, 3, 4 >( leds[ 1 ], c_NumberOfLEDs );
  FastLED.addLeds< APA102, 5, 6 >( leds[ 2 ], c_NumberOfLEDs );
  FastLED.addLeds< APA102, 7, 8 >( leds[ 3 ], c_NumberOfLEDs );
  // Keep power usage sane (at least while testing)
  setMaxPowerInVoltsAndMilliamps( 5 /* V */ , 1000 /* mA */ );
  //initialise
  for( unsigned int i = 0U; i < c_Stripes; i++ )
  {
    for( unsigned int j = 0U; j < c_NumberOfLEDs; j++ )
    {
      leds[ i ][ j ] = CRGB( 0, 0, 0 );
    }
  }
  for( unsigned int i = 0U; i < c_Stripes; i++ )
  {
    knopfGedryckt[ i ] = false;
  }
}

void loop()
{ 
  //jeder steifen
  for( unsigned int i = 0U; i <= c_Stripes; i++ )
  {
    //von hinten
    for( unsigned int j = c_NumberOfLEDs - 1U; j > 0U; j-- )
    {
      //wenn von hinten weiss
      if( leds[ i ][ j ] == CRGB( 255, 255, 255 ) )
      {
        //last LED -> just ignore
        if( j != ( c_NumberOfLEDs - 1U ) )
        {
          //weiss einen nach vorne (j+1) muss existieren
          leds[ i ][ j + 1U ] = CRGB( 255, 255, 255 );
        }
        //fade out
        for( unsigned int fade = 0U; fade < c_FadeTail; fade++ )
        {
          //wenn nicht am anfang des strangs
          if( ( j - 1U ) >= 0U )
          {
            CRGB current = leds[ i ][ j - 1U ];
            //is schon wieder weiss?
            if( current == CRGB( 255, 255, 255 ) )
            {
              //dann nicht mehr faden
              break;
            }
            //um argument verdunkeln - 255 ist max
            current.fadeToBlackBy( ( 255 / c_FadeTail ) * fade );
            leds[ i ][ j ] = current;
          }
          else
          {
            break;
          }
        }
      }
    }
    if( knopfGedryckt[ i ] )
    {
      //farbe waehlen
      CRGB farbe;
      unsigned short hue = 0U;
      if( leds[ i ][ 0 ] == CRGB( 255, 255, 255 ) )
      {
        leds[ i ][ 0 ] = farbe.setHSV( random( 255 ), c_Saturation, c_Value );
      }
      else if( leds[ i ][ 0 ] != CRGB( 0, 0, 0 ) )
      {
        leds[ i ][ 0 ].fadeToBlackBy( 255 / c_FadeTail );
      }
    }
    int currentKnopf = digitalRead( i + c_ButtonOffset );
    //zum ersten mal
    if( ( currentKnopf == HIGH ) && ( knopfGedryckt[i] == false ) )
    {
      //merken
      knopfGedryckt[i] = true;
      //weissen dot am anfang
      leds[ i ][ 0 ] = CRGB( 255, 255, 255 );
    }
    //losgelassen
    else if( ( currentKnopf == LOW )  && ( knopfGedryckt[ i ] == true ) )
    {
      knopfGedryckt[ i ] = false;
    }
    //falls HIGH und gedryckt: ignorieren, da schon alles getan

    //debug out to serial
    if( c_WriteToSerial )
    {
      Serial.print( leds[ i ][ 0 ] );
      Serial.print( "\t" );
      if( i == ( c_Stripes - 1U ) )
      {
        Serial.println( "" );
      }
    }
  }
  FastLED.show();
}
