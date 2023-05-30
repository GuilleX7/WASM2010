import { IoRandomGeneratorHandler } from '@/core/ts/io';
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
