import { IUiIoController } from '@/types';
import { IoButtonsHandler } from '@/wasm/asm2010/io';

export enum TUiButtonsEventName {
  Down = 'down',
  Up = 'up',
}

export type TUiButtonsEvent = {
  type: TUiButtonsEventName;
  buttonIdx: number;
};

export class UiButtonsController
  extends IoButtonsHandler
  implements IUiIoController<TUiButtonsEvent, void>
{
  uiUpdateState(event: TUiButtonsEvent): void {
    switch (event.type) {
      case TUiButtonsEventName.Down:
        if (this.lastButtonPressed === IoButtonsHandler.IoButtonsNonePressed) {
          this.lastButtonPressed = event.buttonIdx;
          this.isStillPressing = true;
        }
        break;
      case TUiButtonsEventName.Up:
        if (this.lastButtonPressed === event.buttonIdx) {
          this.isStillPressing = false;
        }
    }
  }

  uiGetState(): void {}
}
