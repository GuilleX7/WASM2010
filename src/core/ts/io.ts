import {
  CS_IO_READ_NOT_CONTROLLED,
  CS_IO_WRITE_NOT_CONTROLLED,
  CS_IO_WRITE_CONTROLLED,
  TAsm2010IoImports,
} from './types';

export interface ICsIoHandler {
  ioRead(): number;
  ioWrite(content: number): number;
}

export class Asm2010IoManager {
  private registeredHandlers: Record<number, ICsIoHandler> = {};

  public registerHandlers(handlers: Record<number, ICsIoHandler>) {
    this.registeredHandlers = { ...handlers };
  }

  private internalCsIoRead(address: number): number {
    return (
      this.registeredHandlers[address]?.ioRead() ?? CS_IO_READ_NOT_CONTROLLED
    );
  }

  private internalCsIoWrite(address: number, content: number): number {
    return (
      this.registeredHandlers[address]?.ioWrite(content) ??
      CS_IO_WRITE_NOT_CONTROLLED
    );
  }

  public getInternalCsIoFunctions(): TAsm2010IoImports {
    return {
      wasm_custom_io_read: this.internalCsIoRead.bind(this),
      wasm_custom_io_write: this.internalCsIoWrite.bind(this),
    };
  }
}

export class IoHexDisplayHandler implements ICsIoHandler {
  protected content = 0;

  ioRead(): number {
    return this.content;
  }

  ioWrite(content: number): number {
    this.content = content;
    return CS_IO_WRITE_CONTROLLED;
  }
}

export class IoButtonsHandler implements ICsIoHandler {
  static IoButtonsDefaultMode = 0;
  static IoButtonsNonRepeatMode = 1;
  static IoButtonsNonePressed = 0;
  static IoButtonsAPressed = 1;
  static IoButtonsBPressed = 2;
  static IoButtonsCPressed = 4;
  static IoButtonsDPressed = 8;
  static IoButtonsEPressed = 16;
  static IoButtonsFPressed = 32;
  static IoButtonsGPressed = 64;
  static IoButtonsHPressed = 128;

  protected isStillPressing = false;
  protected lastButtonPressed = IoButtonsHandler.IoButtonsNonePressed;
  protected lastButtonRead = IoButtonsHandler.IoButtonsNonePressed;
  protected mode = IoButtonsHandler.IoButtonsDefaultMode;

  ioRead(): number {
    const lastButtonPressed = this.lastButtonPressed;
    if (!this.isStillPressing) {
      this.lastButtonPressed = IoButtonsHandler.IoButtonsNonePressed;
    }
    if (
      this.mode === IoButtonsHandler.IoButtonsNonRepeatMode &&
      lastButtonPressed === this.lastButtonRead
    ) {
      return IoButtonsHandler.IoButtonsNonePressed;
    }
    this.lastButtonRead = lastButtonPressed;
    return lastButtonPressed;
  }

  ioWrite(content: number): number {
    this.mode =
      content & 1
        ? IoButtonsHandler.IoButtonsNonRepeatMode
        : IoButtonsHandler.IoButtonsDefaultMode;
    return CS_IO_WRITE_CONTROLLED;
  }
}

export class IoKeyboardHandler implements ICsIoHandler {
  static IoKeyboardClearBuffer = 0;
  static IoKeyboardPeekMode = 1;
  static IoKeyboardDefaultMode = 2;

  protected buffer = '';
  protected mode = IoKeyboardHandler.IoKeyboardDefaultMode;

  ioRead(): number {
    if (!this.buffer) {
      this.mode = IoKeyboardHandler.IoKeyboardDefaultMode;
      return 0;
    }
    if (this.mode === IoKeyboardHandler.IoKeyboardPeekMode) {
      this.mode = IoKeyboardHandler.IoKeyboardDefaultMode;
      return this.buffer ? 1 : 0;
    }
    const firstUnreadCharacter = this.buffer.charCodeAt(0);
    this.buffer = this.buffer.slice(1);
    return firstUnreadCharacter;
  }

  ioWrite(content: number): number {
    switch (content & 3) {
      case IoKeyboardHandler.IoKeyboardClearBuffer:
        this.buffer = '';
        break;
      case IoKeyboardHandler.IoKeyboardPeekMode:
        this.mode = IoKeyboardHandler.IoKeyboardPeekMode;
        break;
      default:
        this.mode = IoKeyboardHandler.IoKeyboardDefaultMode;
        break;
    }
    return CS_IO_WRITE_CONTROLLED;
  }
}

export class IoRandomGeneratorHandler implements ICsIoHandler {
  ioRead(): number {
    return Math.random() * 256;
  }

  ioWrite(): number {
    return CS_IO_WRITE_CONTROLLED;
  }
}
