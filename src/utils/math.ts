export function positiveMod(number: number, module: number) {
  return ((number % module) + module) % module;
}
