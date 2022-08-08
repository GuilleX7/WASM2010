import { IUiIoController } from '@/types';
import { IoKeyboardHandler } from '@/wasm/asm2010/io';

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
