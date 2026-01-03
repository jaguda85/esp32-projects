import { defineCollection, z } from 'astro:content';

const projectsCollection = defineCollection({
  type: 'content',
  schema: z.object({
    title: z.string(),
    description: z.string(),
    mainImage: z.string(),
    images: z.array(z.string()).optional(),
    date: z.date(),
    tags: z.array(z.string()).optional(),
    components: z.array(z.string()).optional(),
    difficulty: z.enum(['Łatwy', 'Średni', 'Trudny']).optional(),
    leftPins: z.array(z.object({
      pin: z.string(),
      connection: z.string(),
    })).optional(),
    rightPins: z.array(z.object({
      pin: z.string(),
      connection: z.string(),
    })).optional(),
    libraries: z.array(z.object({
      name: z.string(),
      author: z.string().optional(),
      link: z.string().optional(),
    })).optional(),
    // NOWE - kod Arduino
    arduinoCode: z.string().optional(),
    arduinoFilename: z.string().optional(),
  }),
});

export const collections = {
  'projects': projectsCollection,
};