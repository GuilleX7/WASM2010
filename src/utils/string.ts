export function countLines(string: string): number {
  return string.match(/\n/g)?.length ?? 0;
}
