export function maxAmountOfDigitsToRepresentNumber(
  base: number,
  sizeInBits: number
) {
  return Math.ceil(sizeInBits / Math.log2(base));
}

export function stringFilledWith(character: string, length: number) {
  return new Array(length).fill(character).join('');
}

export function padString(
  content: string,
  paddingCharacter: string,
  length: number
) {
  return `${stringFilledWith(paddingCharacter, length)}${content}`.slice(
    -length
  );
}

export function formatNumber(number: number, base: number, sizeInBits: number) {
  const totalCharacters = maxAmountOfDigitsToRepresentNumber(base, sizeInBits);
  const paddedString = padString(number.toString(base), '0', totalCharacters);
  const prefix = base === 16 ? '0x' : '';
  return `${prefix}${paddedString}`;
}
