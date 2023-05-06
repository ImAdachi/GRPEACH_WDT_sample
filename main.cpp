//このプログラムはGR-PEACHを用いてWDT(ウォッチドックタイマ)を
//動作させるサンプルプログラムである

//参考資料
//1. RZ/A1Hグループ，RZ/A1Mグループ ユーザーズマニュアル ハードウェア編 (P562 ~ 572) 
//   https://www.renesas.com/jp/ja/document/mah/rza1h-group-rza1m-group-users-manual-hardware?r=467676
//2. arm Mbed → Questions → ウォッチドックタイマーの使い方。
//   https://os.mbed.com/questions/5684/1418978878/

#include "mbed.h"
#include "wdt_iodefine.h"

Serial pc(USBTX,USBRX,115200);

DigitalIn button(P6_0);//userButton


int main()
{
    //設定値の確認 読みだしの際は8bitにする(P568)
    uint8_t read_wdtwtcnt, read_wdtwtcsr,read_wdtwrcsr;
    read_wdtwtcnt = WDTWTCNT;
    read_wdtwtcsr = WDTWTCSR;
    read_wdtwrcsr = WDTWRCSR;
    pc.printf("WTCNT=%d,WTCSR=%d,WRCSR=%d,\n",read_wdtwtcnt,read_wdtwtcsr,read_wdtwrcsr);

    //WRSRのWOVFがオーバーフローを出したままだったら，0に設定し直す(P566,P567,P572)
    if((read_wdtwrcsr & 0b10000000) == 0b10000000){   
        WDTWTCNT = 0x5A00;//WOVFを設定するときはWTCNTが0xFF以外である必要があるので書き換える(P572)
        WDTWRCSR = 0xA500;
    }

    //WTCSR (P565,566)
    //タイマモードセレクトをウォッチドッグタイマモードに変更
    //クロックセレクトをオーバフロー周期125msになるように111に設定
    //0b01011111 = 0x5F
    WDTWTCSR = 0xA55F;

    //RSTEをWTCNTがオーバフローしたとき､内部リセットするに変更（P566）
    WDTWRCSR = 0x5A5F;

    //カウントを0にする
    WDTWTCNT = 0x5A00;

    //WTCSR の TME ビットに 1 をセットしてカウントスタートする
    WDTWTCSR = 0xA57F;

    read_wdtwtcnt = WDTWTCNT;
    read_wdtwtcsr = WDTWTCSR;
    read_wdtwrcsr = WDTWRCSR;
    pc.printf("WTCNT=%d,WTCSR=%d,WRCSR=%d,\n\n",read_wdtwtcnt,read_wdtwtcsr,read_wdtwrcsr);

    while (true) {
        //オーバーフロー周期内に1度はタイマを0にしないとリセットされてしまう
        //このサンプルプログラムでは125ms
        if(button.read() == true){//ユーザーボタンが押されていない時にカウントを0にする
            read_wdtwtcnt = WDTWTCNT;
            pc.printf("WTCNT = %d\n",read_wdtwtcnt);

            WDTWTCNT = 0x5A00;
        }
        ThisThread::sleep_for(100);
    }
}

