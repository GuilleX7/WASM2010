import { IUiIoController } from '@/types';
import { IoHexDisplayHandler } from '@/wasm/asm2010/io';

export type TUiHexDisplayState = {
  firstDigit: number;
  secondDigit: number;
};

export class UiHexDisplayController
  extends IoHexDisplayHandler
  implements IUiIoController<never, TUiHexDisplayState>
{
  uiUpdateState(input: never): void {}

  uiGetState(): TUiHexDisplayState {
    return {
      firstDigit: this.content & 0xf0,
      secondDigit: this.content & 0x0f,
    };
  }
}
