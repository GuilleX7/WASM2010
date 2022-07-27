export function maxAmountOfDigitsToRepresentNumber(
  radix: number,
  sizeInBits: number
) {
  return Math.ceil(sizeInBits / Math.log2(radix));
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

export function delimiteString(
  string: string,
  chunkSize: number,
  delimiter: string
) {
  return Array.from(string).reduce(
    (accum, currentCharacter, currentCharacterIdx) => {
      return (accum +=
        (currentCharacterIdx + 1) % chunkSize === 0
          ? currentCharacter + delimiter
          : currentCharacter);
    },
    ''
  );
}

export function formatNumber(
  number: number,
  radix: number,
  sizeInBits: number,
  prefix: string = ''
) {
  const totalCharacters = maxAmountOfDigitsToRepresentNumber(radix, sizeInBits);
  const paddedString = padString(number.toString(radix), '0', totalCharacters);
  return `${prefix}${paddedString}`;
}
