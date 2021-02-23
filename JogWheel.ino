/* JogWheel
 * @date    2021-02-13
 * @author  TK303
 */
#include <BleKeyboard.h>
#include "SwitchInput.h"

BleKeyboard bleKeyboard("TK303:JogWheel"); //デバイスの名前を指定できます


/* GPIO入力：ロータリーエンコーダー（ｘ２） */
const int PIN_BUTTON_A = 32;  /* INPUT_PULLUP に設定する。ボタンを押したら LOW */
const int PIN_BUTTON_B = 33;  /* INPUT_PULLUP に設定する。ボタンを押したら LOW */

/* Rotary Encoder input */
int s_rotenc_input_A, s_rotenc_input_B;
int s_prev_rotenc_ptn;

/* loop interval control */
unsigned long s_prev_rotenc_time;

/* port input value of Rotary Encoder */
const int ROTENC_INPUT_NONE = 0x00;
const int ROTENC_INPUT_A    = 0x01;
const int ROTENC_INPUT_B    = 0x02;

/* Rotary Encoder Pattern： 入力１状態 ＝ 2ビット */
const int ROTENC_PTN_0 = ROTENC_INPUT_NONE | ROTENC_INPUT_NONE;
const int ROTENC_PTN_1 = ROTENC_INPUT_NONE | ROTENC_INPUT_A   ;
const int ROTENC_PTN_2 = ROTENC_INPUT_B    | ROTENC_INPUT_NONE;
const int ROTENC_PTN_3 = ROTENC_INPUT_B    | ROTENC_INPUT_A   ;

/*  Rotary Encoder direction (CW/CCW) pattern for 1 click 
 *  （１クリックは4状態 → 2ビットｘ 4状態 ＝ 8ビット）      */
int s_dir_ptn_history;
const int ROTENC_1CLICK_DIR_CW  = (ROTENC_PTN_0 << 6) | (ROTENC_PTN_2 << 4) | (ROTENC_PTN_3 << 2) | (ROTENC_PTN_1 << 0);  /* 0 -> 2 -> 3 -> 1 (-> 0) */
const int ROTENC_1CLICK_DIR_CCW = (ROTENC_PTN_0 << 6) | (ROTENC_PTN_1 << 4) | (ROTENC_PTN_3 << 2) | (ROTENC_PTN_2 << 0);  /* 0 -> 1 -> 3 -> 2 (-> 0) */


/* 実機システムの諸元 */
#define SAMPLING_ROTENC_MICROS (1000)


/******************************************************************************
 * ポート入力を実行するコールバック
 * （一定時間間隔で呼び出される）
 * typedef PortInput_t (*PortInputStub_t)(int index);
 */
static PortInput_t port_input_stub(int index)
{
    PortInput_t input = INPUT_OFF;
    int button = HIGH;
    
    switch(index)
    {
    case BTN_IDX_ORANGE:
        button = digitalRead(PIN_BUTTON_A);
        break;

    case BTN_IDX_YELLOW:
        button = digitalRead(PIN_BUTTON_B);
        break;
    }

    input = (button == LOW) ? INPUT_ON : INPUT_OFF;
    return input;
}


/******************************************************************************
 * チャタリング除去済み入力の通知コールバック
 * （入力確定で呼び出される）
 * typedef void (*SwitchInputNotifier_t)(int index, PortInput_t input);
 */
void sw_input_notifier(int index, PortInput_t input)
{
  switch(index)
  {
  case BTN_IDX_ORANGE:
    s_rotenc_input_A = (input == INPUT_ON) ? ROTENC_INPUT_A : ROTENC_INPUT_NONE;
    break;

  case BTN_IDX_YELLOW:
    s_rotenc_input_B = (input == INPUT_ON) ? ROTENC_INPUT_B : ROTENC_INPUT_NONE;
    break;
  }
}


/**/
void process_rotary_encoder()
{
  /* ロータリーエンコーダーのポート入力をサンプリングする（チャタリング除去も実施） */
  SwitchInput::PollingPort();

  /* 回転方向を判別する：入力1状態は2ビットで表現される */
  int cur_rotenc_ptn;
  cur_rotenc_ptn = s_rotenc_input_B | s_rotenc_input_A;
  
  if (s_prev_rotenc_ptn != cur_rotenc_ptn)
  {
    s_prev_rotenc_ptn = cur_rotenc_ptn;
  }
  else
  { /* 入力に変化がなければ何もしない */
    return;
  }

  /* 入力の履歴から1クリック分の回転方向を特定する 
   *   １クリックは4状態 → 2ビットｘ 4状態 ＝ 8ビット */
  s_dir_ptn_history = (s_dir_ptn_history << 2) | cur_rotenc_ptn;
  int dir_ptn = s_dir_ptn_history & ((1 << 8) - 1);
  if (dir_ptn == ROTENC_1CLICK_DIR_CW)
  {
      bleKeyboard.print("=");    // 右回転：日本語キーボードに "^" を送るには "-" を指定する必要がある
      Serial.println("RIGHT");
  }
  else   if (dir_ptn == ROTENC_1CLICK_DIR_CCW)
  {
      bleKeyboard.print("-");    // 左回転
      Serial.println("LEFT");
  }
  else
  {
    return;  /* do nothing. */
  }
}


/******************************************************************************
 */
void setup() {
  Serial.begin(115200);
  Serial.println("Starting BLE work!");
  bleKeyboard.begin();

  /* ロータリーエンコーダーのポート入力 */
  pinMode(32,INPUT_PULLUP);    //GPIO32を入力に
  pinMode(33,INPUT_PULLUP);    //GPIO33を入力に
  SwitchInput::Init();
  SwitchInput::Register(BTN_IDX_ORANGE, port_input_stub, sw_input_notifier);
  SwitchInput::Register(BTN_IDX_YELLOW, port_input_stub, sw_input_notifier);

  s_prev_rotenc_time = 0;
  s_prev_rotenc_ptn = ROTENC_PTN_0;
  s_dir_ptn_history = 0;
}

/******************************************************************************
 */
void loop() {
  unsigned long cur_time = micros();  /* millis()で 1ms とやると取りこぼしが多いので micros()で 1000us */
  unsigned long interval_rotenc = cur_time - s_prev_rotenc_time;

  /* ロータリーエンコーダーのサンプリングタイミング */
  if (SAMPLING_ROTENC_MICROS < interval_rotenc)
  {
    s_prev_rotenc_time = cur_time;
    process_rotary_encoder();
  }
}

