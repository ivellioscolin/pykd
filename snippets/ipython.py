from IPython.config.loader import Config
from IPython.terminal.embed import InteractiveShellEmbed

from IPython.terminal.interactiveshell import TerminalInteractiveShell

import pykd

cfg = Config()

cfg.InteractiveShell.colors = 'NoColor'
cfg.InteractiveShell.readline_use = False
cfg.InteractiveShell.autoindent = True

cfg.PromptManager.in_template = 'In <\\#>: '
cfg.PromptManager.in2_template = '   .\\D.: '
cfg.PromptManager.out_template = 'Out<\\#>: '

ipshell = InteractiveShellEmbed(config=cfg)

ipshell()    