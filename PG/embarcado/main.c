#include <pic18f4520.h>
#include "lcd.h"
#include "bits.h"
#include "keypad.h"
#include "adc.h"


// t em milisegundos (de 2 em 2 ms)
// valor maximo de 256 x 2ms == 512ms
void delayX2ms(unsigned char t){ 
	unsigned char i, j;
    for(t ;t > 0; t--){
        for(i = 0; i < 50; i++){
            for(j = 0;j < 25; j++);
        }
    }
}

void menuLcd(unsigned char opcao){
    unsigned char i;
    char menu0[10] = "Teste LEDs";
    char menu1[15] = "Teste sinal ext";
    
    // imprime o menu inicial
    
    lcdCommand(0x0); // limpa o display
    lcdCommand(0x1);
    lcdCommand(0x8); // leva para a primeira posicao
    lcdCommand(0x0);
    
    if(opcao){
        lcdData(0x7E);
    }else{
        lcdData(' ');
    }
    
    for(i = 0;i < 11;i++){
        lcdData(menu0[i]);
    }
    
    // proxima linha
    lcdCommand(0xC);
    lcdCommand(0x0);
    
    if(!opcao){
        lcdData(0x7E);
    }else{
        lcdData(' ');
    }
    
    for(i = 0;i < 15;i++){
        lcdData(menu1[i]);
    }
}

void proxMenu(unsigned char opcao){
    lcdCommand(0x8);
    lcdCommand(0x0);
    
    // faz troca a seta de posicao 
    if(opcao){
        lcdData(0x7E);
    }else{
        lcdData(' ');
    }
    
    lcdCommand(0xc);
    lcdCommand(0x0);
    
    if(!opcao){
        lcdData(0x7E);
    }else{
        lcdData(' ');
    }
}

void testeLed(){
    unsigned char i;
    char titulo[13] = "   Teste LEDs";
    
    lcdCommand(0x0); // limpa o display
    lcdCommand(0x1);
    lcdCommand(0x8); // leva para a primeira posicao
    lcdCommand(0x0);
    
    // imprime o titulo
    for(i = 0;i < 13;i++){
        lcdData(titulo[i]);
    }
    
    // acende os LEDs
    TRISB &= 0x0f;
    PORTB |= 0xf0;
    
    // proxima linha
    lcdCommand(0xC);
    lcdCommand(0x0);
    
    // efeito barra de progresso
    for(i = 0;i < 16;i++){
        lcdData(0xFF);
        delayX2ms(50); // aprox 100ms
    }
    
    // apaga os LEDs
    PORTB = 0x00;
}

void testeSinalExt(){
    unsigned char i;
    unsigned int valor, botao = 0x0;
    char titulo[10] = "Sinal ext.";
    char sair[5] = " sair";
    
    lcdCommand(0x0); // limpa o display
    lcdCommand(0x1);
    lcdCommand(0x8); // leva para a primeira posicao
    lcdCommand(0x0);
    
    // imprime o titulo
    for(i = 0;i < 10;i++){
        lcdData(titulo[i]);
    }
    
    // leva para a linha 2
    lcdCommand(0xC); 
    lcdCommand(0x0);
    
    // imprime a opcao de sair
    lcdData(0x7F);
    for(i = 0;i < 5;i++){
        lcdData(sair[i]);
    }
    
    for(;;){
        // leva para a linha 1 coluna 12
        lcdCommand(0x8); 
        lcdCommand(0xb);
        
        // varre o teclado
        kpDebounce();
        
        // valor do canal 0 do ADC
        valor = adcRead(0);
        
        // converte de 0 a 100 para 0 a 3.3
        valor = (valor * 33) / 100;
        
        // atualiza o valor no visor
        lcdData(((valor % 1000)/100) + '0');
        lcdData(',');
        lcdData(((valor % 100)/10) + '0');
        lcdData((valor % 10) + '0');
        lcdData('V');
        
        if(botao != kpRead()){
            botao = kpRead();
            if(botao == 0x04){
                // SETA PARA ESQUERDA (botão 4)
                // vola ao menu principal
                
                return;
            }
        }
    }
}

void main(void) {
    unsigned char opcao = 0x1;
    unsigned int botao = 0x0;
    
    TRISB = 0x00;
    TRISD = 0x00;
    PORTB = 0x00;
    PORTD = 0x00;
    
    adcInit();
    lcdInit();
    kpInit();
    
    menuLcd(opcao);
    
    for(;;){
        
        kpDebounce();
        
        if(botao != kpRead()){
            botao = kpRead();
            
            if(botao == 0x80){
                // SETA PARA CIMA (botão 2)
                // faz a seta caminhar para o proximo menu

                opcao = !opcao;
                proxMenu(opcao);
            }

            if(botao == 0x40){
                // ENTER (botão 5)
                
                if(opcao){
                    testeLed();
                    menuLcd(opcao);
                }else{
                    testeSinalExt();
                    menuLcd(opcao);
                }
            }

            if(botao == 0x20){
                // SETA PARA BAIXO (botão 8)
                // faz a seta caminhar para o proximo menu

                opcao = !opcao;
                proxMenu(opcao);
            }

            if(botao == 0x04){
                // SETA PARA ESQUERDA (botão 4)
                // não faz nada
            }

            if(botao == 0x400){
                // SETA PARA DIREITA (botão 6)
                // age como enter
                
                if(opcao){
                    testeLed();
                    menuLcd(opcao);
                }else{
                    testeSinalExt();
                    menuLcd(opcao);
                }
            }
            
        }
    }
}