export function formatNumber(number: number, base: number, sizeInBits: number) {
  const totalCharacters = Math.ceil(sizeInBits / Math.log2(base));
  const paddedString = `${new Array(totalCharacters)
    .fill('0')
    .join('')}${number.toString(base)}`.slice(-totalCharacters);
  const prefix = base === 16 ? '0x' : '';
  return `${prefix}${paddedString}`;
}
