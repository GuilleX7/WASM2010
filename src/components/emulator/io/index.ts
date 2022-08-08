import { IUiIoController, TCtor } from '@/types';
import { UiButtonsController } from './buttons/ButtonsController';
import { UiHexDisplayController } from './hex-display/HexDisplayController';
import { UiKeyboardController } from './keyboard/KeyboardController';
import { UiRandomGeneratorController } from './random-generator/RandomGeneratorController';

export enum IoComponentId {
  HexDisplay = 'hex-display',
  Buttons = 'buttons',
  RandomGenerator = 'random-generator',
  Keyboard = 'keyboard',
}

export type TIoComponentDefinition = {
  id: IoComponentId;
  controllerComponentName?: string;
  controllerConstructor: TCtor<IUiIoController<unknown, unknown>>;
};

export type TIoComponentInstance = {
  id: IoComponentId;
  controllerComponentName?: string;
  controllerInstance: IUiIoController<unknown, unknown>;
};

export const registeredIoComponents: Record<
  IoComponentId,
  TIoComponentDefinition
> = {
  [IoComponentId.HexDisplay]: {
    id: IoComponentId.HexDisplay,
    controllerComponentName: 'HexDisplayController',
    controllerConstructor: UiHexDisplayController,
  },
  [IoComponentId.Buttons]: {
    id: IoComponentId.Buttons,
    controllerComponentName: 'ButtonsController',
    controllerConstructor: UiButtonsController,
  },
  [IoComponentId.RandomGenerator]: {
    id: IoComponentId.RandomGenerator,
    controllerConstructor: UiRandomGeneratorController,
  },
  [IoComponentId.Keyboard]: {
    id: IoComponentId.Keyboard,
    controllerComponentName: 'KeyboardController',
    controllerConstructor: UiKeyboardController,
  },
};
