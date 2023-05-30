import { Constructor } from '@/types';
import { ICsIoHandler } from '@/core/ts/io';
import { UiButtonsController } from './buttons/ButtonsController';
import { UiHexDisplayController } from './hex-display/HexDisplayController';
import { UiKeyboardController } from './keyboard/KeyboardController';
import { UiRandomGeneratorController } from './random-generator/RandomGeneratorController';

export interface IUiIoController<I, O> extends ICsIoHandler {
  uiUpdateState(input: I): void;
  uiGetState(): O;
}

export enum IoComponentId {
  HexDisplay = 'hex-display',
  Buttons = 'buttons',
  RandomGenerator = 'random-generator',
  Keyboard = 'keyboard',
}

export type TIoComponentDefinition = {
  id: IoComponentId;
  name: string;
  controllerComponentName?: string;
  controllerConstructor: Constructor<IUiIoController<unknown, unknown>>;
};

export type TIoComponentInstance = {
  id: IoComponentId;
  name: string;
  controllerComponentName?: string;
  controllerInstance: IUiIoController<unknown, unknown>;
};

export const registeredIoComponents: Record<
  IoComponentId,
  TIoComponentDefinition
> = {
  [IoComponentId.HexDisplay]: {
    id: IoComponentId.HexDisplay,
    name: 'Hexadecimal display',
    controllerComponentName: 'HexDisplayController',
    controllerConstructor: UiHexDisplayController,
  },
  [IoComponentId.Buttons]: {
    id: IoComponentId.Buttons,
    name: 'Buttons',
    controllerComponentName: 'ButtonsController',
    controllerConstructor: UiButtonsController,
  },
  [IoComponentId.RandomGenerator]: {
    id: IoComponentId.RandomGenerator,
    name: 'Random generator',
    controllerConstructor: UiRandomGeneratorController,
  },
  [IoComponentId.Keyboard]: {
    id: IoComponentId.Keyboard,
    name: 'Keyboard',
    controllerComponentName: 'KeyboardController',
    controllerConstructor: UiKeyboardController,
  },
};
