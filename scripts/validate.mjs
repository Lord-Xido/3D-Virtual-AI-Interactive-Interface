import fs from 'node:fs';
import path from 'node:path';
import crypto from 'node:crypto';
import vm from 'node:vm';
import {fileURLToPath} from 'node:url';

const root=path.resolve(path.dirname(fileURLToPath(import.meta.url)),'..');
const runtimePath=path.join(root,'voxel-video-machine.html');
const lockPath=path.join(root,'ARCHITECTURE_LOCK.json');
const html=fs.readFileSync(runtimePath,'utf8');
const lock=JSON.parse(fs.readFileSync(lockPath,'utf8'));
const failures=[];
const requireText=(needle,label)=>{if(!html.includes(needle))failures.push(`missing ${label}: ${needle}`);};

for(const file of lock.files){if(!fs.existsSync(path.join(root,file)))failures.push(`locked file missing: ${file}`);}
const digest=crypto.createHash('sha256').update(html).digest('hex');
if(digest!==lock.runtime_sha256)failures.push(`runtime sha256 mismatch: expected ${lock.runtime_sha256}, got ${digest}`);

const scripts=[...html.matchAll(/<script(?:\s[^>]*)?>([\s\S]*?)<\/script>/gi)].map(match=>match[1]).filter(source=>source.trim());
if(scripts.length!==1)failures.push(`expected exactly one inline runtime script, found ${scripts.length}`);
for(const [index,source] of scripts.entries()){try{new vm.Script(source,{filename:`voxel-video-machine.inline-${index}.js`});}catch(error){failures.push(`inline JavaScript syntax error: ${error.message}`);}}

requireText("version:'2.0.0'",'runtime version');
requireText('fixedStep:1/60','fixed-step clock');
requireText('maxGpuInstances:12000','bounded GPU pool');
requireText('class SparseVoxelMemory','sparse memory');
requireText('class AdaptiveQualityController','adaptive quality controller');
requireText('class VideoRecorder','video recorder');
requireText('exportSnapshot()','provenance snapshot export');
requireText('webglcontextlost','WebGL context-loss handler');
requireText('spiral-of-death guard','accumulator safety guard');

if(failures.length){console.error('Validation failed:\n- '+failures.join('\n- '));process.exit(1);}
console.log(`Validated ${path.relative(root,runtimePath)} (${digest})`);
console.log(`Architecture lock ${lock.lock_id} is coherent across ${lock.files.length} files.`);
