#pragma once

const char UI_HTML[] = R"rawhtml(
<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>Windhawk Backup Suite</title>
  <style>
    :root {
      --oled-black: #000000;
      --card-bg: rgba(14, 15, 17, 0.85);
      --card-elevated: rgba(22, 24, 28, 0.95);
      --card-hover: rgba(28, 31, 37, 0.95);
      --border-subtle: rgba(255, 255, 255, 0.1);
      --border-hover: rgba(255, 255, 255, 0.22);
      --neon-green: #00f076;
      --neon-green-glow: rgba(0, 240, 118, 0.28);
      --neon-green-subtle: rgba(0, 240, 118, 0.12);
      --cyber-yellow: #ffd600;
      --cyber-yellow-glow: rgba(255, 214, 0, 0.28);
      --cyber-yellow-subtle: rgba(255, 214, 0, 0.12);
      --white: #ffffff;
      --white-glow: rgba(255, 255, 255, 0.2);
      --text-muted: #a1a1aa;
      --text-dim: #71717a;
      --radius-pill: 9999px;
      --radius-card: 26px;
      --radius-item: 16px;
    }

    * {
      box-sizing: border-box;
      margin: 0;
      padding: 0;
      user-select: none;
      -webkit-user-select: none;
    }

    body {
      font-family: 'Segoe UI Variable Text', -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, sans-serif;
      background-color: var(--oled-black);
      color: var(--white);
      min-height: 100vh;
      display: flex;
      flex-direction: column;
      overflow-x: hidden;
      -webkit-font-smoothing: antialiased;
      background-image: 
        radial-gradient(circle at 50% -10%, rgba(0, 240, 118, 0.08) 0%, transparent 45%),
        radial-gradient(circle at 100% 100%, rgba(255, 214, 0, 0.05) 0%, transparent 40%);
    }

    /* Tactile Press & Release Animation */
    button,
    .card-pill-btn,
    .btn-neon-action,
    .btn-alert-action,
    .btn-pill-toggle,
    .btn-round-back,
    .btn-secondary-pill,
    .btn-browse-pill {
      transition: transform 0.15s cubic-bezier(0.16, 1, 0.3, 1), background 0.15s ease, border-color 0.15s ease, box-shadow 0.15s ease;
    }

    button:active,
    .card-pill-btn:active,
    .btn-neon-action:active,
    .btn-alert-action:active,
    .btn-pill-toggle:active,
    .btn-round-back:active,
    .btn-secondary-pill:active,
    .btn-browse-pill:active {
      transform: scale(0.96) !important;
      transition: transform 0.06s ease !important;
    }

    /* Top Fluid Header */
    .app-header {
      height: 56px;
      display: flex;
      align-items: center;
      justify-content: space-between;
      padding: 0 28px;
      background: rgba(8, 8, 10, 0.7);
      backdrop-filter: blur(20px);
      border-bottom: 1px solid var(--border-subtle);
    }

    .brand-wrap {
      display: flex;
      align-items: center;
      gap: 12px;
    }

    .brand-icon-wrap {
      width: 30px;
      height: 30px;
      border-radius: var(--radius-pill);
      background: var(--neon-green-subtle);
      border: 1px solid rgba(0, 240, 118, 0.3);
      color: var(--neon-green);
      display: flex;
      align-items: center;
      justify-content: center;
      box-shadow: 0 0 12px var(--neon-green-glow);
    }

    .brand-title {
      font-size: 14.5px;
      font-weight: 700;
      color: var(--white);
      letter-spacing: -0.3px;
      text-shadow: 0 0 16px var(--white-glow);
    }

    .header-status-pill {
      display: flex;
      align-items: center;
      gap: 9px;
      padding: 6px 16px;
      background: rgba(15, 15, 18, 0.8);
      border: 1px solid var(--border-subtle);
      border-radius: var(--radius-pill);
      font-size: 12.5px;
      color: var(--white);
      backdrop-filter: blur(12px);
    }

    .status-dot-pulse {
      width: 8px;
      height: 8px;
      border-radius: 50%;
      background: var(--neon-green);
      box-shadow: 0 0 10px var(--neon-green);
    }

    .status-dot-pulse.inactive {
      background: #ef4444;
      box-shadow: 0 0 10px #ef4444;
    }

    /* Main Container */
    .main-stage {
      flex: 1;
      display: flex;
      flex-direction: column;
      padding: 32px 36px;
      max-width: 920px;
      margin: 0 auto;
      width: 100%;
    }

    /* Screen Transitions */
    .screen {
      display: none;
      flex: 1;
      flex-direction: column;
      animation: fluidSlideUp 0.24s cubic-bezier(0.16, 1, 0.3, 1) forwards;
    }

    .screen.active {
      display: flex;
    }

    @keyframes fluidSlideUp {
      from { opacity: 0; transform: translateY(6px); }
      to { opacity: 1; transform: translateY(0); }
    }

    /* Hero Section */
    .hero-banner {
      margin-bottom: 32px;
      margin-top: 10px;
    }

    .hero-heading {
      font-size: 30px;
      font-weight: 800;
      color: var(--white);
      letter-spacing: -0.6px;
      margin-bottom: 8px;
      text-shadow: 0 0 24px rgba(255, 255, 255, 0.15);
    }

    .hero-subtext {
      font-size: 14px;
      color: var(--text-muted);
      line-height: 1.55;
      max-width: 620px;
    }

    /* Action Cards Grid */
    .action-grid {
      display: grid;
      grid-template-columns: 1fr 1fr;
      gap: 22px;
      margin-bottom: 28px;
    }

    .fluid-card {
      background: var(--card-bg);
      border: 1px solid var(--border-subtle);
      border-radius: var(--radius-card);
      padding: 30px 26px;
      display: flex;
      flex-direction: column;
      cursor: pointer;
      position: relative;
      overflow: hidden;
      backdrop-filter: blur(24px);
      transition: all 0.22s cubic-bezier(0.16, 1, 0.3, 1);
    }

    .fluid-card:hover {
      background: var(--card-elevated);
      transform: translateY(-2px);
    }

    .fluid-card.export-tile:hover {
      border-color: rgba(0, 240, 118, 0.45);
      box-shadow: 0 16px 36px -10px rgba(0, 0, 0, 0.7), 0 0 24px var(--neon-green-glow);
    }

    .fluid-card.import-tile:hover {
      border-color: rgba(255, 214, 0, 0.45);
      box-shadow: 0 16px 36px -10px rgba(0, 0, 0, 0.7), 0 0 24px var(--cyber-yellow-glow);
    }

    .card-icon-pill {
      width: 50px;
      height: 50px;
      border-radius: var(--radius-pill);
      display: flex;
      align-items: center;
      justify-content: center;
      margin-bottom: 20px;
      transition: transform 0.22s ease;
    }

    .fluid-card:hover .card-icon-pill {
      transform: scale(1.08);
    }

    .export-tile .card-icon-pill {
      background: var(--neon-green-subtle);
      color: var(--neon-green);
      border: 1px solid rgba(0, 240, 118, 0.35);
      box-shadow: 0 0 14px var(--neon-green-glow);
    }

    .import-tile .card-icon-pill {
      background: var(--cyber-yellow-subtle);
      color: var(--cyber-yellow);
      border: 1px solid rgba(255, 214, 0, 0.35);
      box-shadow: 0 0 14px var(--cyber-yellow-glow);
    }

    .card-icon-pill svg {
      width: 25px;
      height: 25px;
    }

    .card-title-text {
      font-size: 18.5px;
      font-weight: 700;
      color: var(--white);
      margin-bottom: 8px;
      text-shadow: 0 0 12px rgba(255, 255, 255, 0.12);
    }

    .card-body-text {
      font-size: 13.5px;
      color: var(--text-muted);
      line-height: 1.5;
      margin-bottom: 26px;
      flex: 1;
    }

    .card-pill-btn {
      display: inline-flex;
      align-items: center;
      justify-content: space-between;
      padding: 12px 20px;
      border-radius: var(--radius-pill);
      font-size: 13px;
      font-weight: 700;
      transition: all 0.2s ease;
    }

    .export-tile .card-pill-btn {
      background: rgba(0, 240, 118, 0.08);
      border: 1px solid rgba(0, 240, 118, 0.25);
      color: var(--neon-green);
    }

    .export-tile:hover .card-pill-btn {
      background: var(--neon-green);
      border-color: var(--neon-green);
      color: #000000;
      box-shadow: 0 0 18px var(--neon-green-glow);
    }

    .import-tile .card-pill-btn {
      background: rgba(255, 214, 0, 0.08);
      border: 1px solid rgba(255, 214, 0, 0.25);
      color: var(--cyber-yellow);
    }

    .import-tile:hover .card-pill-btn {
      background: var(--cyber-yellow);
      border-color: var(--cyber-yellow);
      color: #000000;
      box-shadow: 0 0 18px var(--cyber-yellow-glow);
    }

    /* Missing Windhawk Alert Banner */
    .alert-banner {
      display: none;
      background: rgba(255, 214, 0, 0.06);
      border: 1px solid rgba(255, 214, 0, 0.3);
      border-radius: var(--radius-card);
      padding: 16px 22px;
      align-items: center;
      justify-content: space-between;
      margin-top: auto;
      box-shadow: 0 0 20px var(--cyber-yellow-subtle);
    }

    .alert-banner.show {
      display: flex;
    }

    .alert-left-col {
      display: flex;
      align-items: center;
      gap: 14px;
    }

    .alert-warn-icon {
      color: var(--cyber-yellow);
      display: flex;
      filter: drop-shadow(0 0 6px var(--cyber-yellow-glow));
    }

    .alert-headline {
      font-size: 13.5px;
      font-weight: 700;
      color: #fffbeb;
    }

    .alert-caption {
      font-size: 12px;
      color: #fde68a;
      opacity: 0.85;
    }

    .btn-alert-action {
      background: var(--cyber-yellow);
      color: #000000;
      padding: 9px 18px;
      border-radius: var(--radius-pill);
      font-size: 12.5px;
      font-weight: 700;
      border: none;
      cursor: pointer;
      display: inline-flex;
      align-items: center;
      gap: 7px;
      box-shadow: 0 0 14px var(--cyber-yellow-glow);
      transition: all 0.2s ease;
    }

    /* Inner Screen Header */
    .screen-header-bar {
      display: flex;
      align-items: center;
      gap: 16px;
      margin-bottom: 22px;
    }

    .btn-round-back {
      width: 38px;
      height: 38px;
      border-radius: var(--radius-pill);
      background: var(--card-bg);
      border: 1px solid var(--border-subtle);
      color: var(--white);
      display: flex;
      align-items: center;
      justify-content: center;
      cursor: pointer;
      transition: all 0.2s ease;
    }

    .btn-round-back:hover {
      background: var(--card-hover);
      border-color: rgba(255, 255, 255, 0.3);
    }

    .screen-title-text {
      font-size: 20px;
      font-weight: 700;
      color: var(--white);
      text-shadow: 0 0 16px var(--white-glow);
    }

    .screen-subtitle-text {
      font-size: 13px;
      color: var(--text-muted);
    }

    /* Toolbar */
    .fluid-toolbar {
      display: flex;
      align-items: center;
      justify-content: space-between;
      gap: 14px;
      margin-bottom: 16px;
    }

    .fluid-search-pill {
      flex: 1;
      position: relative;
    }

    .fluid-search-pill input {
      width: 100%;
      background: var(--card-bg);
      border: 1px solid var(--border-subtle);
      border-radius: var(--radius-pill);
      padding: 10px 16px 10px 42px;
      color: var(--white);
      font-size: 13.5px;
      outline: none;
      transition: all 0.2s ease;
    }

    .fluid-search-pill input:focus {
      border-color: var(--neon-green);
      box-shadow: 0 0 16px var(--neon-green-glow);
    }

    .fluid-search-pill svg {
      position: absolute;
      left: 14px;
      top: 50%;
      transform: translateY(-50%);
      width: 16px;
      height: 16px;
      color: var(--text-dim);
    }

    .btn-pill-toggle {
      padding: 10px 20px;
      border-radius: var(--radius-pill);
      background: var(--card-bg);
      border: 1px solid var(--border-subtle);
      color: var(--white);
      font-size: 13px;
      font-weight: 600;
      cursor: pointer;
      transition: all 0.2s ease;
      white-space: nowrap;
    }

    .btn-pill-toggle:hover {
      background: var(--card-hover);
      border-color: rgba(255, 255, 255, 0.3);
    }

    /* Mod Selection Cards (No Zoom / No Jump on Hover) */
    .mod-cards-scroll {
      flex: 1;
      max-height: 400px;
      overflow-y: auto;
      display: flex;
      flex-direction: column;
      gap: 10px;
      padding-right: 6px;
    }

    .mod-cards-scroll::-webkit-scrollbar {
      width: 6px;
    }
    .mod-cards-scroll::-webkit-scrollbar-thumb {
      background: rgba(255, 255, 255, 0.15);
      border-radius: var(--radius-pill);
    }

    .mod-item-card {
      background: var(--card-bg);
      border: 1px solid var(--border-subtle);
      border-radius: var(--radius-item);
      padding: 14px 18px;
      display: flex;
      align-items: center;
      gap: 16px;
      cursor: pointer;
      transition: background 0.15s ease, border-color 0.15s ease;
    }

    .mod-item-card:hover {
      background: var(--card-hover);
      border-color: rgba(255, 255, 255, 0.22);
    }

    .mod-item-card.selected {
      border-color: rgba(0, 240, 118, 0.4);
      background: rgba(0, 240, 118, 0.05);
      box-shadow: 0 4px 16px rgba(0, 240, 118, 0.08);
    }

    .fluid-checkbox {
      width: 22px;
      height: 22px;
      border-radius: 7px;
      border: 2px solid var(--text-dim);
      display: flex;
      align-items: center;
      justify-content: center;
      flex-shrink: 0;
      transition: all 0.15s ease;
    }

    .mod-item-card.selected .fluid-checkbox {
      background: var(--neon-green);
      border-color: var(--neon-green);
      box-shadow: 0 0 10px var(--neon-green-glow);
    }

    .fluid-checkbox svg {
      width: 14px;
      height: 14px;
      color: #000000;
      display: none;
    }

    .mod-item-card.selected .fluid-checkbox svg {
      display: block;
    }

    .mod-meta-block {
      flex: 1;
      min-width: 0;
    }

    .mod-heading-line {
      display: flex;
      align-items: center;
      gap: 9px;
      margin-bottom: 4px;
    }

    .mod-title-main {
      font-size: 14.5px;
      font-weight: 700;
      color: var(--white);
      white-space: nowrap;
      overflow: hidden;
      text-overflow: ellipsis;
    }

    .mod-pill-tag {
      font-size: 11px;
      font-weight: 600;
      padding: 2px 8px;
      border-radius: var(--radius-pill);
      background: rgba(255, 255, 255, 0.06);
      color: var(--text-muted);
    }

    .mod-state-pill {
      font-size: 10.5px;
      font-weight: 700;
      text-transform: uppercase;
      letter-spacing: 0.5px;
      padding: 2px 9px;
      border-radius: var(--radius-pill);
    }

    .mod-state-pill.active {
      background: var(--neon-green-subtle);
      color: var(--neon-green);
      border: 1px solid rgba(0, 240, 118, 0.25);
    }

    .mod-state-pill.disabled {
      background: rgba(255, 255, 255, 0.05);
      color: var(--text-dim);
    }

    .mod-desc-line {
      font-size: 12.5px;
      color: var(--text-muted);
      white-space: nowrap;
      overflow: hidden;
      text-overflow: ellipsis;
      line-height: 1.4;
    }

    /* Screen Sticky Bottom */
    .fluid-footer-bar {
      margin-top: 20px;
      padding-top: 16px;
      border-top: 1px solid var(--border-subtle);
      display: flex;
      align-items: center;
      justify-content: space-between;
    }

    .footer-counter-label {
      font-size: 13.5px;
      color: var(--text-muted);
    }

    .footer-counter-label strong {
      color: var(--neon-green);
    }

    /* Buttons */
    .btn-neon-action {
      padding: 10px 24px;
      border-radius: var(--radius-pill);
      background: var(--neon-green);
      color: #000000;
      font-size: 13.5px;
      font-weight: 700;
      border: none;
      cursor: pointer;
      display: inline-flex;
      align-items: center;
      gap: 7px;
      box-shadow: 0 0 16px var(--neon-green-glow);
    }

    .btn-neon-action:hover:not(:disabled) {
      box-shadow: 0 0 24px var(--neon-green-glow);
    }

    .btn-neon-action:disabled {
      opacity: 0.35;
      cursor: not-allowed;
      box-shadow: none;
    }

    .btn-secondary-pill {
      padding: 10px 20px;
      border-radius: var(--radius-pill);
      background: var(--card-bg);
      border: 1px solid var(--border-subtle);
      color: var(--white);
      font-size: 13.5px;
      font-weight: 600;
      cursor: pointer;
    }

    .btn-secondary-pill:hover {
      background: var(--card-hover);
      border-color: rgba(255, 255, 255, 0.3);
    }

    /* Configure Save Form */
    .fluid-form-card {
      background: var(--card-bg);
      border: 1px solid var(--border-subtle);
      border-radius: var(--radius-card);
      padding: 28px;
      max-width: 620px;
      margin: 0 auto;
      width: 100%;
      backdrop-filter: blur(24px);
    }

    .fluid-field {
      margin-bottom: 22px;
    }

    .fluid-label {
      font-size: 13.5px;
      font-weight: 700;
      color: var(--white);
      margin-bottom: 8px;
      display: block;
    }

    .fluid-caption {
      font-size: 12px;
      color: var(--text-muted);
      margin-top: 6px;
    }

    .zip-input-box {
      display: flex;
      align-items: center;
      background: rgba(8, 8, 10, 0.9);
      border: 1px solid var(--border-subtle);
      border-radius: var(--radius-pill);
      overflow: hidden;
      transition: all 0.2s ease;
    }

    .zip-input-box:focus-within {
      border-color: var(--neon-green);
      box-shadow: 0 0 16px var(--neon-green-glow);
    }

    .zip-input-box input {
      flex: 1;
      background: transparent;
      border: none;
      padding: 11px 18px;
      color: var(--white);
      font-size: 14px;
      outline: none;
    }

    .zip-input-box span {
      padding: 0 16px;
      font-size: 13px;
      font-weight: 700;
      color: var(--neon-green);
      background: var(--neon-green-subtle);
      height: 100%;
      display: flex;
      align-items: center;
      border-left: 1px solid var(--border-subtle);
    }

    .folder-row-wrap {
      display: flex;
      gap: 10px;
    }

    .folder-row-wrap input {
      flex: 1;
      background: rgba(8, 8, 10, 0.9);
      border: 1px solid var(--border-subtle);
      border-radius: var(--radius-pill);
      padding: 10px 16px;
      color: var(--text-muted);
      font-size: 13px;
      outline: none;
    }

    .btn-browse-pill {
      padding: 10px 18px;
      border-radius: var(--radius-pill);
      background: rgba(255, 255, 255, 0.06);
      border: 1px solid var(--border-subtle);
      color: var(--white);
      font-size: 13px;
      font-weight: 600;
      cursor: pointer;
      white-space: nowrap;
    }

    .btn-browse-pill:hover {
      background: rgba(255, 255, 255, 0.12);
      border-color: rgba(255, 255, 255, 0.3);
    }

    /* Result Confirmation Card */
    .fluid-result-card {
      max-width: 560px;
      margin: 32px auto;
      background: var(--card-bg);
      border: 1px solid rgba(0, 240, 118, 0.35);
      border-radius: var(--radius-card);
      padding: 36px 32px;
      text-align: center;
      backdrop-filter: blur(24px);
      box-shadow: 0 18px 48px -10px rgba(0, 0, 0, 0.8), 0 0 28px var(--neon-green-glow);
    }

    .result-halo-icon {
      width: 60px;
      height: 60px;
      border-radius: var(--radius-pill);
      background: var(--neon-green-subtle);
      border: 1px solid rgba(0, 240, 118, 0.4);
      color: var(--neon-green);
      display: flex;
      align-items: center;
      justify-content: center;
      margin: 0 auto 18px auto;
      box-shadow: 0 0 20px var(--neon-green-glow);
    }

    .result-halo-icon svg {
      width: 32px;
      height: 32px;
    }

    .result-main-title {
      font-size: 22px;
      font-weight: 800;
      color: var(--white);
      margin-bottom: 8px;
      text-shadow: 0 0 16px var(--white-glow);
    }

    .result-body-desc {
      font-size: 13.5px;
      color: var(--text-muted);
      margin-bottom: 22px;
      line-height: 1.5;
    }

    .file-path-capsule {
      background: rgba(8, 8, 10, 0.9);
      border: 1px solid var(--border-subtle);
      border-radius: 14px;
      padding: 13px 16px;
      text-align: left;
      font-size: 12.5px;
      color: var(--text-muted);
      margin-bottom: 26px;
      word-break: break-all;
    }

    .file-path-capsule strong {
      color: var(--white);
      display: block;
      margin-bottom: 4px;
      font-size: 13px;
    }

    /* Import Dropzone */
    .fluid-dropzone {
      border: 2px dashed rgba(255, 214, 0, 0.35);
      background: var(--card-bg);
      border-radius: var(--radius-card);
      padding: 50px 28px;
      text-align: center;
      cursor: pointer;
      transition: all 0.22s ease;
      max-width: 580px;
      margin: 26px auto;
      width: 100%;
      backdrop-filter: blur(20px);
    }

    .fluid-dropzone:hover {
      border-color: var(--cyber-yellow);
      background: rgba(255, 214, 0, 0.04);
      box-shadow: 0 0 24px var(--cyber-yellow-glow);
    }

    .dropzone-halo-icon {
      width: 52px;
      height: 52px;
      border-radius: var(--radius-pill);
      background: var(--cyber-yellow-subtle);
      color: var(--cyber-yellow);
      display: flex;
      align-items: center;
      justify-content: center;
      margin: 0 auto 16px auto;
      border: 1px solid rgba(255, 214, 0, 0.4);
      box-shadow: 0 0 16px var(--cyber-yellow-glow);
    }

    .dropzone-halo-icon svg {
      width: 26px;
      height: 26px;
    }

    .dropzone-title {
      font-size: 16px;
      font-weight: 700;
      color: var(--white);
      margin-bottom: 6px;
    }

    .dropzone-sub {
      font-size: 13px;
      color: var(--text-muted);
      margin-bottom: 20px;
    }

    /* ========================================================
       FLUID LOADING SCREEN & MOVING PATLI PROGRESS BAR
       ======================================================== */
    .fluid-loading-box {
      max-width: 480px;
      margin: 70px auto;
      background: var(--card-bg);
      border: 1px solid var(--border-subtle);
      border-radius: var(--radius-card);
      padding: 46px 36px;
      text-align: center;
      backdrop-filter: blur(24px);
      box-shadow: 0 18px 48px -10px rgba(0, 0, 0, 0.8), 0 0 24px var(--cyber-yellow-glow);
    }

    .loading-halo-wrap {
      width: 60px;
      height: 60px;
      border-radius: var(--radius-pill);
      background: var(--cyber-yellow-subtle);
      border: 1px solid rgba(255, 214, 0, 0.4);
      color: var(--cyber-yellow);
      display: flex;
      align-items: center;
      justify-content: center;
      margin: 0 auto 18px auto;
      box-shadow: 0 0 20px var(--cyber-yellow-glow);
      animation: pulseGlow 1.2s ease-in-out infinite alternate;
    }

    @keyframes pulseGlow {
      from { transform: scale(0.96); box-shadow: 0 0 12px var(--cyber-yellow-glow); }
      to { transform: scale(1.05); box-shadow: 0 0 26px var(--cyber-yellow-glow); }
    }

    .loading-halo-wrap svg {
      width: 28px;
      height: 28px;
    }

    .loading-title-text {
      font-size: 22px;
      font-weight: 800;
      color: var(--white);
      margin-bottom: 6px;
      text-shadow: 0 0 16px var(--white-glow);
      letter-spacing: -0.3px;
    }

    .loading-subtext {
      font-size: 13.5px;
      color: var(--text-muted);
      margin-bottom: 26px;
      line-height: 1.4;
    }

    /* The Smooth Moving Patli Progress Bar */
    .fluid-progress-track {
      width: 100%;
      height: 3px;
      background: rgba(255, 255, 255, 0.08);
      border-radius: var(--radius-pill);
      overflow: hidden;
      position: relative;
    }

    .fluid-progress-bar {
      position: absolute;
      top: 0;
      left: 0;
      bottom: 0;
      width: 38%;
      background: linear-gradient(90deg, transparent, var(--cyber-yellow), var(--neon-green), transparent);
      border-radius: var(--radius-pill);
      box-shadow: 0 0 12px var(--neon-green);
      animation: fluidStream 1.05s cubic-bezier(0.4, 0, 0.2, 1) infinite;
    }

    @keyframes fluidStream {
      0% { transform: translateX(-100%); }
      100% { transform: translateX(330%); }
    }
  </style>
</head>
<body>

  <!-- Top Fluid Header -->
  <header class="app-header">
    <div class="brand-wrap">
      <div class="brand-icon-wrap">
        <svg width="16" height="16" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2.5">
          <path d="M12 2L2 7l10 5 10-5-10-5zM2 17l10 5 10-5M2 12l10 5 10-5"/>
        </svg>
      </div>
      <span class="brand-title">Windhawk Backup Suite</span>
    </div>

    <div class="header-status-pill">
      <span id="header-status-dot" class="status-dot-pulse"></span>
      <span id="header-status-text">Detecting Engine...</span>
    </div>
  </header>

  <main class="main-stage">

    <!-- SCREEN 1: HOME -->
    <section id="screen-home" class="screen active">
      <div class="hero-banner">
        <h1 class="hero-heading">Windhawk Backup Suite</h1>
        <p class="hero-subtext">Instant, high-speed backup and restoration for Windhawk customizations, source code, and registry configurations.</p>
      </div>

      <div class="action-grid">
        <!-- Export Tile -->
        <div class="fluid-card export-tile" onclick="goToExportSelect()">
          <div class="card-icon-pill">
            <svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2">
              <path d="M21 15v4a2 2 0 0 1-2 2H5a2 2 0 0 1-2-2v-4"></path>
              <polyline points="17 8 12 3 7 8"></polyline>
              <line x1="12" y1="3" x2="12" y2="15"></line>
            </svg>
          </div>
          <h2 class="card-title-text">Export Mods (Backup)</h2>
          <p class="card-body-text">Capture all your active and disabled mods, source codes, and registry tweaks into a single portable .zip archive.</p>
          <div class="card-pill-btn">
            <span>Create New Backup</span>
            <span>&rarr;</span>
          </div>
        </div>

        <!-- Import Tile -->
        <div class="fluid-card import-tile" onclick="goToImportFile()">
          <div class="card-icon-pill">
            <svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2">
              <path d="M21 15v4a2 2 0 0 1-2 2H5a2 2 0 0 1-2-2v-4"></path>
              <polyline points="7 10 12 15 17 10"></polyline>
              <line x1="12" y1="15" x2="12" y2="3"></line>
            </svg>
          </div>
          <h2 class="card-title-text">Import Mods (Restore)</h2>
          <p class="card-body-text">Restore mods and preferences from an existing archive file with selective checklist installation.</p>
          <div class="card-pill-btn">
            <span>Restore from File</span>
            <span>&rarr;</span>
          </div>
        </div>
      </div>

      <!-- Missing Windhawk Alert Banner -->
      <div id="windhawk-alert-banner" class="alert-banner">
        <div class="alert-left-col">
          <div class="alert-warn-icon">
            <svg width="22" height="22" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2">
              <circle cx="12" cy="12" r="10"></circle>
              <line x1="12" y1="8" x2="12" y2="12"></line>
              <line x1="12" y1="16" x2="12.01" y2="16"></line>
            </svg>
          </div>
          <div>
            <div class="alert-headline">Windhawk is not installed on this PC</div>
            <div class="alert-caption">Download and install the official Windhawk setup with one click to begin customizing.</div>
          </div>
        </div>
        <button class="btn-alert-action" onclick="sendToHost('install_windhawk')">
          <svg width="14" height="14" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2.5">
            <path d="M21 15v4a2 2 0 0 1-2 2H5a2 2 0 0 1-2-2v-4"></path>
            <polyline points="7 10 12 15 17 10"></polyline>
            <line x1="12" y1="15" x2="12" y2="3"></line>
          </svg>
          <span>Install Windhawk</span>
        </button>
      </div>
    </section>

    <!-- SCREEN 2: EXPORT MOD SELECTION -->
    <section id="screen-export-select" class="screen">
      <div class="screen-header-bar">
        <button class="btn-round-back" onclick="goToHome()">
          <svg width="16" height="16" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2.5">
            <line x1="19" y1="12" x2="5" y2="12"></line>
            <polyline points="12 19 5 12 12 5"></polyline>
          </svg>
        </button>
        <div>
          <h2 class="screen-title-text">Select Mods to Backup</h2>
          <p class="screen-subtitle-text">All mods are pre-selected. Toggle any mod to include or exclude it.</p>
        </div>
      </div>

      <div class="fluid-toolbar">
        <div class="fluid-search-pill">
          <svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2">
            <circle cx="11" cy="11" r="8"></circle>
            <line x1="21" y1="21" x2="16.65" y2="16.65"></line>
          </svg>
          <input type="text" id="export-search" placeholder="Search mods by name or author..." oninput="filterExportMods()">
        </div>
        <button class="btn-pill-toggle" onclick="toggleAllExportMods()">
          <span id="btn-toggle-all-text">Deselect All</span>
        </button>
      </div>

      <div class="mod-cards-scroll" id="export-mods-list">
        <!-- Rendered dynamically -->
      </div>

      <div class="fluid-footer-bar">
        <div class="footer-counter-label">
          <span id="export-selected-count">0</span> of <span id="export-total-count">0</span> mods selected
        </div>
        <button id="btn-export-next" class="btn-neon-action" onclick="goToExportSave()">
          <span>Next: Name & Save</span>
          <span>&rarr;</span>
        </button>
      </div>
    </section>

    <!-- SCREEN 3: EXPORT NAME & DESTINATION -->
    <section id="screen-export-save" class="screen">
      <div class="screen-header-bar">
        <button class="btn-round-back" onclick="showScreen('screen-export-select')">
          <svg width="16" height="16" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2.5">
            <line x1="19" y1="12" x2="5" y2="12"></line>
            <polyline points="12 19 5 12 12 5"></polyline>
          </svg>
        </button>
        <div>
          <h2 class="screen-title-text">Name Your Backup</h2>
          <p class="screen-subtitle-text">Personalize your archive name and choose the destination location.</p>
        </div>
      </div>

      <div class="fluid-form-card">
        <div class="fluid-field">
          <label class="fluid-label" for="backup-name-input">Archive Name</label>
          <div class="zip-input-box">
            <input type="text" id="backup-name-input" value="Windhawk_Backup">
            <span>.zip</span>
          </div>
          <p class="fluid-caption">You can give your backup any custom name (e.g., My_Taskbar_Setup, OLED_Black_Theme).</p>
        </div>

        <div class="fluid-field">
          <label class="fluid-label">Destination Directory</label>
          <div class="folder-row-wrap">
            <input type="text" id="destination-folder-input" readonly>
            <button class="btn-browse-pill" onclick="browseDestinationFolder()">Browse Folder...</button>
          </div>
        </div>

        <div class="fluid-field" style="margin-bottom: 0;">
          <label class="fluid-label">Included Mods (<span id="recap-mod-count">0</span>)</label>
          <div id="recap-mod-chips" style="display: flex; flex-wrap: wrap; gap: 8px; max-height: 85px; overflow-y: auto; padding: 4px 0;"></div>
        </div>
      </div>

      <div class="fluid-footer-bar" style="max-width: 620px; margin: 20px auto 0 auto; width: 100%;">
        <button class="btn-secondary-pill" onclick="showScreen('screen-export-select')">Back</button>
        <button id="btn-create-backup" class="btn-neon-action" onclick="executeBackup()">
          <span>Create Backup Archive</span>
        </button>
      </div>
    </section>

    <!-- SCREEN 4: EXPORT SUCCESS -->
    <section id="screen-export-success" class="screen">
      <div class="fluid-result-card">
        <div class="result-halo-icon">
          <svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2.5">
            <polyline points="20 6 9 17 4 12"></polyline>
          </svg>
        </div>
        <h2 class="result-main-title">Backup Created Successfully!</h2>
        <p class="result-body-desc">All selected mods, source files, and registry settings have been safely bundled into your portable zip archive.</p>

        <div class="file-path-capsule">
          <strong>Archive Location:</strong>
          <span id="success-backup-path">C:\...</span>
        </div>

        <div style="display: flex; gap: 12px; justify-content: center;">
          <button class="btn-secondary-pill" onclick="openExportFolder()">Show in Explorer</button>
          <button class="btn-neon-action" onclick="goToHome()">Done</button>
        </div>
      </div>
    </section>

    <!-- SCREEN 5: IMPORT SELECT FILE -->
    <section id="screen-import-file" class="screen">
      <div class="screen-header-bar">
        <button class="btn-round-back" onclick="goToHome()">
          <svg width="16" height="16" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2.5">
            <line x1="19" y1="12" x2="5" y2="12"></line>
            <polyline points="12 19 5 12 12 5"></polyline>
          </svg>
        </button>
        <div>
          <h2 class="screen-title-text">Import Backup Archive</h2>
          <p class="screen-subtitle-text">Select a previously created Windhawk backup archive to review and restore.</p>
        </div>
      </div>

      <div class="fluid-dropzone" onclick="browseZipFile()">
        <div class="dropzone-halo-icon">
          <svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2">
            <path d="M21 15v4a2 2 0 0 1-2 2H5a2 2 0 0 1-2-2v-4"></path>
            <polyline points="7 10 12 15 17 10"></polyline>
            <line x1="12" y1="15" x2="12" y2="3"></line>
          </svg>
        </div>
        <h3 class="dropzone-title">Select Windhawk Backup Archive (.zip)</h3>
        <p class="dropzone-sub">Click anywhere here to browse for a backup file</p>
        <button class="btn-alert-action" onclick="event.stopPropagation(); browseZipFile();">Browse Backup File...</button>
      </div>
    </section>

    <!-- SCREEN 6: IMPORT SELECT MODS -->
    <section id="screen-import-select" class="screen">
      <div class="screen-header-bar">
        <button class="btn-round-back" onclick="showScreen('screen-import-file')">
          <svg width="16" height="16" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2.5">
            <line x1="19" y1="12" x2="5" y2="12"></line>
            <polyline points="12 19 5 12 12 5"></polyline>
          </svg>
        </button>
        <div>
          <h2 class="screen-title-text">Review & Restore Mods</h2>
          <p class="screen-subtitle-text" id="import-archive-info">Archive details loaded.</p>
        </div>
      </div>

      <div class="fluid-toolbar">
        <div class="fluid-search-pill">
          <svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2">
            <circle cx="11" cy="11" r="8"></circle>
            <line x1="21" y1="21" x2="16.65" y2="16.65"></line>
          </svg>
          <input type="text" id="import-search" placeholder="Filter mods inside backup..." oninput="filterImportMods()">
        </div>
        <button class="btn-pill-toggle" onclick="toggleAllImportMods()">
          <span id="btn-toggle-all-import-text">Deselect All</span>
        </button>
      </div>

      <div class="mod-cards-scroll" id="import-mods-list">
        <!-- Rendered dynamically -->
      </div>

      <div style="margin-top: 14px; display: flex; align-items: center; gap: 10px; font-size: 13px; color: var(--text-muted);">
        <input type="checkbox" id="restart-windhawk-checkbox" checked style="accent-color: var(--neon-green); width: 17px; height: 17px; cursor: pointer;">
        <label for="restart-windhawk-checkbox" style="cursor: pointer;">Restart Windhawk automatically after import to activate restored mods</label>
      </div>

      <div class="fluid-footer-bar">
        <div class="footer-counter-label">
          <span id="import-selected-count">0</span> of <span id="import-total-count">0</span> mods selected to restore
        </div>
        <button id="btn-execute-restore" class="btn-neon-action" onclick="executeRestore()">
          <span>Restore Selected Mods</span>
        </button>
      </div>
    </section>

    <!-- SCREEN 7: FLUID LOADING & MOVING PATLI PROGRESS BAR -->
    <section id="screen-loading" class="screen">
      <div class="fluid-loading-box">
        <div class="loading-halo-wrap">
          <svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2.2">
            <path d="M21 15v4a2 2 0 0 1-2 2H5a2 2 0 0 1-2-2v-4"></path>
            <polyline points="7 10 12 15 17 10"></polyline>
            <line x1="12" y1="15" x2="12" y2="3"></line>
          </svg>
        </div>
        <h2 class="loading-title-text" id="loading-title">Importing...</h2>
        <p class="loading-subtext" id="loading-sub">Reading archive and verifying mods...</p>
        
        <!-- Moving Patli Fluid Progress Bar -->
        <div class="fluid-progress-track">
          <div class="fluid-progress-bar"></div>
        </div>
      </div>
    </section>

    <!-- SCREEN 8: IMPORT SUCCESS -->
    <section id="screen-import-success" class="screen">
      <div class="fluid-result-card">
        <div class="result-halo-icon">
          <svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2.5">
            <polyline points="20 6 9 17 4 12"></polyline>
          </svg>
        </div>
        <h2 class="result-main-title">Mods Restored Successfully!</h2>
        <p class="result-body-desc">The selected mods and their registry configurations were imported into Windhawk. Windhawk has been refreshed.</p>

        <div style="display: flex; gap: 12px; justify-content: center;">
          <button class="btn-neon-action" onclick="goToHome()">Back to Home</button>
        </div>
      </div>
    </section>

  </main>

  <script>
    // State
    let systemInfo = {
      isInstalled: true,
      installPath: "",
      mods: [],
      defaultDesktop: ""
    };

    let exportSelection = new Set();
    let importModsData = [];
    let importSelection = new Set();
    let selectedZipPath = "";
    let currentBackupFilePath = "";

    function sendToHost(action, data = {}) {
      if (window.chrome && window.chrome.webview) {
        window.chrome.webview.postMessage({ action, ...data });
      }
    }

    function showScreen(screenId) {
      document.querySelectorAll(".screen").forEach(s => s.classList.remove("active"));
      const target = document.getElementById(screenId);
      if (target) target.classList.add("active");
    }

    function showLoading(title, subtitle) {
      document.getElementById("loading-title").textContent = title || "Importing...";
      document.getElementById("loading-sub").textContent = subtitle || "Processing, please wait...";
      showScreen("screen-loading");
    }

    function goToHome() {
      showScreen("screen-home");
      sendToHost("refresh_mods");
    }

    // Host IPC
    window.onVaultMessage = function(data) {
      if (data.type === "init") {
        systemInfo = data;
        updateHomeUI();
      } else if (data.type === "inspect_started") {
        // Zip was chosen, show fluid moving progress bar immediately!
        showLoading("Importing...", "Reading archive and verifying mods...");
      } else if (data.type === "destination_folder_selected") {
        document.getElementById("destination-folder-input").value = data.folder;
      } else if (data.type === "zip_selected") {
        handleZipSelected(data);
      } else if (data.type === "backup_complete") {
        handleBackupComplete(data);
      } else if (data.type === "restore_complete") {
        handleRestoreComplete(data);
      } else if (data.type === "error") {
        showScreen("screen-home");
        alert(data.message || "An unexpected error occurred.");
      }
    };

    function updateHomeUI() {
      const dot = document.getElementById("header-status-dot");
      const text = document.getElementById("header-status-text");
      const alertBanner = document.getElementById("windhawk-alert-banner");

      if (systemInfo.isInstalled) {
        dot.classList.remove("inactive");
        text.textContent = `Windhawk Active (${systemInfo.mods.length} mods)`;
        alertBanner.classList.remove("show");
      } else {
        dot.classList.add("inactive");
        text.textContent = "Windhawk Not Detected";
        alertBanner.classList.add("show");
      }

      // Default desktop
      if (!document.getElementById("destination-folder-input").value && systemInfo.defaultDesktop) {
        document.getElementById("destination-folder-input").value = systemInfo.defaultDesktop;
      }
    }

    /* EXPORT FLOW */
    function goToExportSelect() {
      exportSelection.clear();
      systemInfo.mods.forEach(m => exportSelection.add(m.id));
      renderExportModsList();
      showScreen("screen-export-select");
    }

    function renderExportModsList() {
      const container = document.getElementById("export-mods-list");
      const search = (document.getElementById("export-search").value || "").toLowerCase();
      container.innerHTML = "";

      const filtered = systemInfo.mods.filter(m => 
        (m.name || '').toLowerCase().includes(search) || 
        (m.author || '').toLowerCase().includes(search) ||
        (m.id || '').toLowerCase().includes(search)
      );

      if (filtered.length === 0) {
        container.innerHTML = `<div style="text-align: center; padding: 40px; color: var(--text-dim); font-size: 13px;">No mods match your search.</div>`;
      } else {
        filtered.forEach(m => {
          const isSelected = exportSelection.has(m.id);
          const row = document.createElement("div");
          row.className = "mod-item-card" + (isSelected ? " selected" : "");
          row.onclick = () => toggleExportMod(m.id);

          row.innerHTML = `
            <div class="fluid-checkbox">
              <svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="3.5">
                <polyline points="20 6 9 17 4 12"></polyline>
              </svg>
            </div>
            <div class="mod-meta-block">
              <div class="mod-heading-line">
                <span class="mod-title-main">${m.name}</span>
                <span class="mod-pill-tag">${m.version ? 'v' + m.version : 'v1.0'}</span>
                ${m.author ? `<span class="mod-pill-tag">${m.author}</span>` : ''}
                <span class="mod-state-pill ${m.disabled ? 'disabled' : 'active'}">${m.disabled ? 'Disabled' : 'Active'}</span>
              </div>
              <div class="mod-desc-line">${m.description || m.id}</div>
            </div>
          `;
          container.appendChild(row);
        });
      }

      updateExportCounters();
    }

    function toggleExportMod(id) {
      if (exportSelection.has(id)) {
        exportSelection.delete(id);
      } else {
        exportSelection.add(id);
      }
      renderExportModsList();
    }

    function toggleAllExportMods() {
      if (exportSelection.size === systemInfo.mods.length) {
        exportSelection.clear();
      } else {
        systemInfo.mods.forEach(m => exportSelection.add(m.id));
      }
      renderExportModsList();
    }

    function filterExportMods() {
      renderExportModsList();
    }

    function updateExportCounters() {
      const selected = exportSelection.size;
      const total = systemInfo.mods.length;

      document.getElementById("export-selected-count").textContent = selected;
      document.getElementById("export-total-count").textContent = total;

      const toggleBtn = document.getElementById("btn-toggle-all-text");
      toggleBtn.textContent = (selected === total && total > 0) ? "Deselect All" : "Select All";

      document.getElementById("btn-export-next").disabled = (selected === 0);
    }

    function goToExportSave() {
      if (exportSelection.size === 0) return;

      const today = new Date().toISOString().slice(0, 10);
      const nameInput = document.getElementById("backup-name-input");
      if (!nameInput.value || nameInput.value.startsWith("Windhawk_Backup")) {
        nameInput.value = `Windhawk_Backup_${today}`;
      }

      const recapChips = document.getElementById("recap-mod-chips");
      recapChips.innerHTML = "";
      document.getElementById("recap-mod-count").textContent = exportSelection.size;

      systemInfo.mods.forEach(m => {
        if (exportSelection.has(m.id)) {
          const chip = document.createElement("span");
          chip.className = "mod-pill-tag";
          chip.style.background = "var(--neon-green-subtle)";
          chip.style.color = "var(--neon-green)";
          chip.style.border = "1px solid rgba(0, 240, 118, 0.3)";
          chip.textContent = m.name;
          recapChips.appendChild(chip);
        }
      });

      showScreen("screen-export-save");
    }

    function browseDestinationFolder() {
      sendToHost("browse_destination_folder");
    }

    function executeBackup() {
      const folder = document.getElementById("destination-folder-input").value.trim();
      let name = document.getElementById("backup-name-input").value.trim();

      if (!name) {
        alert("Please enter a name for the archive.");
        return;
      }
      if (!folder) {
        alert("Please select a destination directory.");
        return;
      }

      if (name.endsWith(".zip")) name = name.slice(0, -4);
      const targetZip = `${folder}\\${name}.zip`;

      showLoading("Creating Backup Archive...", "Packing source code, registry keys, and manifest into portable zip...");

      sendToHost("create_backup", {
        selectedModIds: Array.from(exportSelection),
        targetZipPath: targetZip
      });
    }

    function handleBackupComplete(data) {
      currentBackupFilePath = data.path;
      document.getElementById("success-backup-path").textContent = data.path;
      showScreen("screen-export-success");
    }

    function openExportFolder() {
      if (currentBackupFilePath) {
        sendToHost("open_folder", { path: currentBackupFilePath });
      }
    }

    /* IMPORT FLOW */
    function goToImportFile() {
      showScreen("screen-import-file");
    }

    function browseZipFile() {
      sendToHost("browse_zip_file");
    }

    function handleZipSelected(data) {
      selectedZipPath = data.path;
      importModsData = data.manifest.mods || [];
      importSelection.clear();
      importModsData.forEach(m => importSelection.add(m.id));

      document.getElementById("import-archive-info").textContent = 
        `Archive created on ${data.manifest.created_at || 'unknown date'} • ${importModsData.length} mods available.`;

      renderImportModsList();
      showScreen("screen-import-select");
    }

    function renderImportModsList() {
      const container = document.getElementById("import-mods-list");
      const search = (document.getElementById("import-search").value || "").toLowerCase();
      container.innerHTML = "";

      const filtered = importModsData.filter(m => 
        (m.name || '').toLowerCase().includes(search) || 
        (m.author || '').toLowerCase().includes(search) ||
        (m.id || '').toLowerCase().includes(search)
      );

      if (filtered.length === 0) {
        container.innerHTML = `<div style="text-align: center; padding: 40px; color: var(--text-dim); font-size: 13px;">No mods inside this archive match your search.</div>`;
      } else {
        filtered.forEach(m => {
          const isSelected = importSelection.has(m.id);
          const row = document.createElement("div");
          row.className = "mod-item-card" + (isSelected ? " selected" : "");
          row.onclick = () => toggleImportMod(m.id);

          row.innerHTML = `
            <div class="fluid-checkbox">
              <svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="3.5">
                <polyline points="20 6 9 17 4 12"></polyline>
              </svg>
            </div>
            <div class="mod-meta-block">
              <div class="mod-heading-line">
                <span class="mod-title-main">${m.name || m.id}</span>
                <span class="mod-pill-tag">${m.version ? 'v' + m.version : 'v1.0'}</span>
                ${m.author ? `<span class="mod-pill-tag">${m.author}</span>` : ''}
              </div>
              <div class="mod-desc-line">${m.description || m.id}</div>
            </div>
          `;
          container.appendChild(row);
        });
      }

      updateImportCounters();
    }

    function toggleImportMod(id) {
      if (importSelection.has(id)) {
        importSelection.delete(id);
      } else {
        importSelection.add(id);
      }
      renderImportModsList();
    }

    function toggleAllImportMods() {
      if (importSelection.size === importModsData.length) {
        importSelection.clear();
      } else {
        importModsData.forEach(m => importSelection.add(m.id));
      }
      renderImportModsList();
    }

    function filterImportMods() {
      renderImportModsList();
    }

    function updateImportCounters() {
      const selected = importSelection.size;
      const total = importModsData.length;

      document.getElementById("import-selected-count").textContent = selected;
      document.getElementById("import-total-count").textContent = total;

      const toggleBtn = document.getElementById("btn-toggle-all-import-text");
      toggleBtn.textContent = (selected === total && total > 0) ? "Deselect All" : "Select All";

      document.getElementById("btn-execute-restore").disabled = (selected === 0);
    }

    function executeRestore() {
      if (importSelection.size === 0) return;

      const restart = document.getElementById("restart-windhawk-checkbox").checked;
      
      showLoading("Importing...", "Restoring mods and configuring Windhawk preferences...");

      sendToHost("restore_backup", {
        zipPath: selectedZipPath,
        selectedModIds: Array.from(importSelection),
        restartWindhawk: restart
      });
    }

    function handleRestoreComplete(data) {
      showScreen("screen-import-success");
    }

    // Ready signal
    window.addEventListener("DOMContentLoaded", () => {
      sendToHost("app_ready");
    });
  </script>
</body>
</html>
)rawhtml";
