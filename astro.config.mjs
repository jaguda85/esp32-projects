// @ts-check
import { defineConfig } from 'astro/config';

import vercel from '@astrojs/vercel';

// https://astro.build/config
export default defineConfig({
  site: 'https://jaguda85.github.io',
  base: '/esp32-projects',
  adapter: vercel(),
});