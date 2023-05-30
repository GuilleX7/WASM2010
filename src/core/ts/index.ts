import { Asm2010Instance, Asm2010InstanceBuilder } from './logic';
import { CsPlatform } from './types';

let asm2010Instance: Asm2010Instance;

export async function loadAsm2010Instance(
  csPlatform?: CsPlatform
): Promise<void> {
  asm2010Instance = await new Asm2010InstanceBuilder().createInstance(
    csPlatform
  );
}

export function getAsm2010Instance(): Asm2010Instance {
  return asm2010Instance;
}
