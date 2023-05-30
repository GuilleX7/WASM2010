import { useGlobalStore } from './global';

export function getStoreLocalStorageKey(storeId: string): string {
  return `${storeId}_state`;
}

export function restoreStoreState(
  storeId: string
): Record<string, unknown> | null {
  try {
    const savedState = localStorage.getItem(getStoreLocalStorageKey(storeId));
    if (savedState) {
      return JSON.parse(savedState);
    }
  } catch {
    return null;
  }

  return null;
}

export function saveStoreState(
  storeId: string,
  state: Record<string, unknown>
): void {
  const storeLocalStorageKey = getStoreLocalStorageKey(storeId);
  
  try {
    localStorage.setItem(storeLocalStorageKey, JSON.stringify(state));
  } catch {
    localStorage.removeItem(storeLocalStorageKey);
  }
}

export { useGlobalStore as globalStore };
