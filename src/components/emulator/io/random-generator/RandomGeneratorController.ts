import { IUiIoController } from '@/types';
import { IoRandomGeneratorHandler } from '@/wasm/asm2010/io';

export class UiRandomGeneratorController
  extends IoRandomGeneratorHandler
  implements IUiIoController<never, void>
{
  uiUpdateState(): void {}

  uiGetState(): void {}
}
