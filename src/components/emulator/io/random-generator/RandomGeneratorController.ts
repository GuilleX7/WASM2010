import { IoRandomGeneratorHandler } from '@/asm2010/wrapper/io';
import { IUiIoController }  from '..';

export class UiRandomGeneratorController
  extends IoRandomGeneratorHandler
  implements IUiIoController<never, void>
{
  uiUpdateState(): void {
    /* Stub method */
  }

  uiGetState(): void {
    /* Stub method */
  }
}
