import fs from 'node:fs';
import path from 'node:path';
import vm from 'node:vm';
import {fileURLToPath} from 'node:url';

const root=path.resolve(path.dirname(fileURLToPath(import.meta.url)),'..');
const runtimePath=path.join(root,'autopoietic-multimodal-chat-engine.html');
const html=fs.readFileSync(runtimePath,'utf8');
const failures=[];
const requireText=(needle,label)=>{if(!html.includes(needle))failures.push(`missing ${label}: ${needle}`);};

const scripts=[...html.matchAll(/<script(?:\s[^>]*)?>([\s\S]*?)<\/script>/gi)]
  .map(match=>match[1])
  .filter(source=>source.trim());

if(scripts.length!==1)failures.push(`expected exactly one inline runtime script, found ${scripts.length}`);
for(const [index,source] of scripts.entries()){
  try{new vm.Script(source,{filename:`autopoietic.inline-${index}.js`});}
  catch(error){failures.push(`inline JavaScript syntax error: ${error.message}`);}
}

requireText("version:'2.0.0'",'runtime version');
requireText('class RNG','deterministic RNG');
requireText('class MultimodalLattice','3D lattice');
requireText('class MultimodalCompiler','compiler');
requireText('class MultimodalInterpreter','interpreter');
requireText('class MultimodalFeedback','feedback path');
requireText('class AutopoieticChatEngine','chat engine');
requireText('ResizeObserver','responsive rendering');
requireText('simulation:true','simulation scope marker');
requireText('Export Runtime Snapshot','snapshot control');
requireText('gridSize:8','bounded lattice dimension');
requireText('bytecodeWidth:60','bounded latent bytecode width');

if(failures.length){
  console.error('Validation failed:\n- '+failures.join('\n- '));
  process.exit(1);
}

console.log(`Validated ${path.relative(root,runtimePath)}: deterministic RNG, bounded 8³ lattice, 60-channel compiler, feedback loop, responsive renderer and simulation scope are present.`);
