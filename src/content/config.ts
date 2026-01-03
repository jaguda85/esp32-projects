import { defineCollection, z } from 'astro:content';

// PROJEKTY
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
    arduinoCode: z.string().optional(),
    arduinoFilename: z.string().optional(),
    libraries: z.array(z.object({
      name: z.string(),
      author: z.string().optional(),
      link: z.string().optional(),
    })).optional(),
  }),
});

// FILMY YOUTUBE
const videosCollection = defineCollection({
  type: 'content',
  schema: z.object({
    title: z.string(),
    description: z.string(),
    youtubeId: z.string(), // np. "Vz_z4-nF4J8"
    thumbnail: z.string().optional(),
    date: z.date(),
    tags: z.array(z.string()).optional(),
    relatedProject: z.string().optional(), // slug projektu
  }),
});

// LINKI
const linksCollection = defineCollection({
  type: 'content',
  schema: z.object({
    title: z.string(),
    description: z.string(),
    url: z.string(),
    category: z.enum(['Projekt', 'Biblioteka', 'Tutorial', 'Narzędzie', 'Dokumentacja', 'Inne']),
    image: z.string().optional(),
    date: z.date(),
    tags: z.array(z.string()).optional(),
  }),
});

export const collections = {
  'projects': projectsCollection,
  'videos': videosCollection,
  'links': linksCollection,
};