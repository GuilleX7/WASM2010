import { IoHexDisplayHandler } from '@/core/ts/io';
import { IUiIoController } from '..';

export type TUiHexDisplayState = {
  firstDigit: number;
  secondDigit: number;
};

export class UiHexDisplayController
  extends IoHexDisplayHandler
  implements IUiIoController<never, TUiHexDisplayState>
{
  uiUpdateState(): void {
    /* Stub method */
  }

  uiGetState(): TUiHexDisplayState {
    return {
      firstDigit: this.content & 0xf0,
      secondDigit: this.content & 0x0f,
    };
  }
}
