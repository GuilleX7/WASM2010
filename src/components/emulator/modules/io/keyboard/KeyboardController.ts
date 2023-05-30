import { IoKeyboardHandler } from '@/core/ts/io';
import { IUiIoController } from '..';

export class UiKeyboardController
  extends IoKeyboardHandler
  implements IUiIoController<string, string>
{
  uiUpdateState(char: string): void {
    this.buffer += char;
  }

  uiGetState(): string {
    return this.buffer;
  }
}
