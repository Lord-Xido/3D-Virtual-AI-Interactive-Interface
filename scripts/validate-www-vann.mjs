import fs from 'node:fs';
import path from 'node:path';
import vm from 'node:vm';
import {fileURLToPath} from 'node:url';

const root=path.resolve(path.dirname(fileURLToPath(import.meta.url)),'..');
const runtimePath=path.join(root,'world-wide-web-vann.html');
const html=fs.readFileSync(runtimePath,'utf8');
const failures=[];
const requireText=(needle,label)=>{if(!html.includes(needle))failures.push(`missing ${label}: ${needle}`);};

const scripts=[...html.matchAll(/<script(?:\s[^>]*)?>([\s\S]*?)<\/script>/gi)].map(match=>match[1]).filter(source=>source.trim());
if(scripts.length!==1)failures.push(`expected exactly one inline runtime script, found ${scripts.length}`);
for(const [index,source] of scripts.entries()){
  try{new vm.Script(source,{filename:`world-wide-web-vann.inline-${index}.js`});}
  catch(error){failures.push(`inline JavaScript syntax error: ${error.message}`);}
}

requireText("version:'1.0.0'",'runtime version');
requireText('fixedStep:1/60','fixed-step clock');
requireText('class AutoEncoder3D','autoencoder implementation');
requireText('class WebResourceGraph','Web graph implementation');
requireText('class TransactionalRuntime','transactional runtime');
requireText('this.model.snapshot()','pre-mutation snapshot');
requireText('this.model.restore(this.tx.preWeights)','rollback restoration');
requireText("'Project Λ'",'Lambda projection stage');
requireText("'Update Ω candidate'",'Omega correction stage');
requireText('Export Snapshot','snapshot control');
requireText('maxResources:96','bounded resource graph');

if(failures.length){console.error('Validation failed:\n- '+failures.join('\n- '));process.exit(1);}
console.log(`Validated ${path.relative(root,runtimePath)}: one inline script, deterministic fixed-step autoencoder, Λ gate and rollback invariants present.`);
