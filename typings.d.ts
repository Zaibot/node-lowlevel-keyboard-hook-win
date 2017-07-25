export type Callback = (trigger: 'up' | 'down', key: string) => void;
export function on(modes: string, callback: Callback): void;
export function stop(): void;
