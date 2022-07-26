export function chunks<T>(array: T[], chunkSize: number): T[][] {
  const chunks: T[][] = []
  if (chunkSize) {
    for (let i = 0; i < array.length; i += chunkSize) {
      chunks.push(array.slice(i, i + chunkSize))
    }
  }
  return chunks
}
