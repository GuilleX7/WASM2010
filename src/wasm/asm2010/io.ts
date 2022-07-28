export const CS_IO_READ_NOT_CONTROLLED = -1;
export const CS_IO_WRITE_NOT_CONTROLLED = false;
export const CS_IO_READ_NOT_ALLOWED = 0;
export const CS_IO_WRITE_NOT_ALLOWED = true;
export const CS_IO_WRITE_SUCCESS = true;

export interface ICsIoHandler {
    ioRead(): number;
    ioWrite(content: number): boolean;
}

let registeredIoHandlers: Record<number, ICsIoHandler> = {};

export function csRegisterIoHandlers(handlers: Record<number, ICsIoHandler>) {
    registeredIoHandlers = handlers;
}

export function wasm_custom_io_read(address: number): number {
    return registeredIoHandlers[address]?.ioRead() ?? CS_IO_READ_NOT_CONTROLLED;
}

export function wasm_custom_io_write(
    address: number,
    content: number
): boolean {
    return (
        registeredIoHandlers[address]?.ioWrite(content) ??
        CS_IO_WRITE_NOT_CONTROLLED
    );
}

export class IoHexDisplayHandler implements ICsIoHandler {
    protected content = 0;

    ioRead(): number {
        return this.content;
    }

    ioWrite(content: number): boolean {
        this.content = content;
        return CS_IO_WRITE_SUCCESS;
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
        if (lastButtonPressed === this.lastButtonRead) {
            return IoButtonsHandler.IoButtonsNonePressed;
        }
        this.lastButtonRead = lastButtonPressed;
        return lastButtonPressed;
    }

    ioWrite(content: number): boolean {
        this.mode =
            content & 1
                ? IoButtonsHandler.IoButtonsNonRepeatMode
                : IoButtonsHandler.IoButtonsDefaultMode;
        return CS_IO_WRITE_SUCCESS;
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

    ioWrite(content: number): boolean {
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
        return CS_IO_WRITE_SUCCESS;
    }
}

export class IoRandomGeneratorHandler implements ICsIoHandler {
    ioRead(): number {
        return Math.random() * 256;
    }

    ioWrite(): boolean {
        return CS_IO_WRITE_SUCCESS;
    }
}
