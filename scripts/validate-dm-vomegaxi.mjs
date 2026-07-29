import fs from 'node:fs';
import path from 'node:path';
import vm from 'node:vm';
import {fileURLToPath} from 'node:url';

const root = path.resolve(path.dirname(fileURLToPath(import.meta.url)), '..');
const runtimePath = path.join(root, 'dm-vomegaxi-qsol-engine.html');
const html = fs.readFileSync(runtimePath, 'utf8');
const failures = [];

const requireText = (needle, label) => {
  if (!html.includes(needle)) failures.push(`missing ${label}: ${needle}`);
};

const inlineScripts = [...html.matchAll(/<script(?:\s[^>]*)?>([\s\S]*?)<\/script>/gi)]
  .map(match => match[1])
  .filter(source => source.trim());

for (const [index, source] of inlineScripts.entries()) {
  try {
    new vm.Script(source, {filename: `dm-vomegaxi.inline-${index}.js`});
  } catch (error) {
    failures.push(`inline JavaScript syntax error: ${error.message}`);
  }
}

const runtimeSource = inlineScripts.find(source => source.includes('class InwardMetaEvolution'));
if (!runtimeSource) {
  failures.push('bounded meta-evolution runtime script was not found');
} else {
  const marker = 'window.addEventListener("load",init,{once:true});';
  if (!runtimeSource.includes(marker)) {
    failures.push('runtime load marker was not found for deterministic instrumentation');
  } else {
    const instrumented = runtimeSource.replace(
      marker,
      'window.__DMVOMEGAXI__=Object.freeze({RUNTIME,InwardMetaEvolution});'
    );
    const sandbox = {
      window: {},
      performance: {now: () => 0},
      console,
    };
    vm.createContext(sandbox);

    try {
      new vm.Script(instrumented, {filename: 'dm-vomegaxi.instrumented.js'}).runInContext(sandbox);
      const api = sandbox.window.__DMVOMEGAXI__;
      if (!api) throw new Error('instrumented runtime API was not exposed');

      const sampleCount = 5 * 2048;
      const first = new api.InwardMetaEvolution(sampleCount);
      const second = new api.InwardMetaEvolution(sampleCount);

      if (api.RUNTIME.latentCells !== 512 || api.RUNTIME.latentSide !== 8) {
        failures.push('latent memory contract must remain 8^3 = 512 cells');
      }
      if (api.RUNTIME.logicalCoordinates !== 1_000_000_000) {
        failures.push('logical QSOL coordinate label changed unexpectedly');
      }
      if (first.byteEstimate() >= 1024 * 1024) {
        failures.push(`desktop numerical state exceeds 1 MiB: ${first.byteEstimate()} bytes`);
      }
      if (first.objective !== second.objective) {
        failures.push('fixed-seed initialization is not deterministic');
      }
      if (
        Array.from(first.state.slice(0, 256)).join(',') !==
        Array.from(second.state.slice(0, 256)).join(',')
      ) {
        failures.push('fixed-seed state samples are not deterministic');
      }

      let previousObjective = first.objective;
      for (let iteration = 0; iteration < 8; iteration += 1) {
        const result = first.step(1, 1);
        if (!result.accepted) {
          failures.push(`descent fixture rolled back at iteration ${iteration + 1}`);
          break;
        }
        if (!(first.objective < previousObjective)) {
          failures.push(`objective did not decrease at iteration ${iteration + 1}`);
          break;
        }
        previousObjective = first.objective;
      }

      first.lockFixedPoint();
      if (first.objective !== 0 || first.lastUpdateRms !== 0) {
        failures.push('fixed-point projection did not produce zero objective and update RMS');
      }
      if (first.memory.some(value => value !== 0)) {
        failures.push('fixed-point projection did not clear latent memory');
      }
    } catch (error) {
      failures.push(`instrumented numerical validation failed: ${error.message}`);
    }
  }
}

requireText('fixedStep:1/30', 'fixed-step clock');
requireText('maxSubSteps:3', 'spiral-of-death bound');
requireText('latentCells:512', '8^3 latent memory');
requireText('acceptanceRho:1e-5', 'objective descent gate');
requireText('candidateObjective<=oldObjective', 'transactional commit condition');
requireText('rejectionDamping:.65', 'rollback memory damping');
requireText('webglcontextlost', 'WebGL context-loss handling');
requireText('visibilitychange', 'background lifecycle handling');
requireText('prefers-reduced-motion', 'reduced-motion handling');
requireText('exportSnapshot', 'snapshot export');
requireText('logical visualization label, not a billion allocated cores', 'logical versus resident boundary');

if (failures.length) {
  console.error(`DM-vOmegaXi validation failed:\n- ${failures.join('\n- ')}`);
  process.exit(1);
}

console.log('Validated dm-vomegaxi-qsol-engine.html');
console.log('Deterministic replay, objective descent, fixed point and sub-1-MiB numerical state are coherent.');
